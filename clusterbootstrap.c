#include "clusterbootstrap.h"


int main(int argc, char *argv[])
{
	if(argc > 1)
	{
		fprintf(stderr, "Logfile: %s\n", argv[1]);
	}

	timeout.tv_sec = 1;

	clusterboot_socket();
	if(!sock)
	{
		fprintf(stderr,
		        "Failed to initailize the UDP socket for clusterboot\n");
		return 1;
	}

	SEND_REQUEST: send_broadcast();

	while(!get_response())
	{
		increase_timeout();

		send_broadcast();
	}

	if(!handle_response())
	{
		sleep(10);
		timeout.tv_sec = 1;
		goto SEND_REQUEST;
	}

	close(sock);

	return 0;
}


int clusterboot_socket(void)
{
	struct sockaddr_in address;
	int optval = 1;

	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(sock == -1)
	{
		fprintf(stderr, "Failed to create socket: %m\n");
		return 0;
	}

	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(8008);

	memset(&destination, 0, sizeof(destination));
	destination.sin_family = AF_INET;
	destination.sin_addr.s_addr = 0xffffffff;
	destination.sin_port = htons(8007);

	if(setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &optval,
	              sizeof(optval)) == -1)
	{
		fprintf(stderr, "Failed to set SO_BROADCAST on socket: %m\n");
		close(sock);
		return 0;
	}

	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

	if(bind(sock, (struct sockaddr *)&address, sizeof(address)) == -1)
	{
		fprintf(stderr, "Failed to bind socket: %m\n");
		close(sock);
		return 0;
	}

	return 1;
}


void send_broadcast(void)
{
	int ret;
	ret = sendto(sock, REQUEST, sizeof(REQUEST), 0, (struct sockaddr *)&destination,
	             sizeof(destination));

	if(ret == -1)
	{
		fprintf(stderr, "Error sending broadcast: %m\n");
	}
}


int get_response(void)
{
	ssize_t ret;
	ret = recv(sock, &response, RESPONSE_MAXLEN - 1, 0);
	if(ret == -1)
	{
		return 0;
	}
	else
	{
		response[ret] = '\0';
		return 1;
	}
}


void increase_timeout(void)
{
	timeout.tv_sec *= 2;
	if(timeout.tv_sec > TIMEOUT_MAX)
	{
		timeout.tv_sec = TIMEOUT_MAX;
	}

	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
}


int handle_response(void)
{
	pid_t child_pid;
	int exit_code;

	// Perform wget
	child_pid = fork();
	if(child_pid == 0)
	{
		if(execlp("wget", "wget", response, "-O", "/tmp/bootstrap.sh", NULL) == -1)
		{
			exit(1);
		}
	}
	else
	{
		wait(&exit_code);
		if(exit_code)
		{
			fprintf(stderr, "wget failed\n");
			return 0;
		}
	}

	// Run script
	child_pid = fork();
	if(child_pid == 0)
	{
		if(execlp("bash", "bash", "/tmp/bootstrap.sh", NULL) == -1)
		{
			exit(1);
		}
	}
	else
	{
		wait(&exit_code);
		if(exit_code)
		{
			fprintf(stderr, "bootstrap script failed\n");
			return 0;
		}
	}

	return 1;
}
