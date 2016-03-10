#ifndef __CLUSTERBOOTSTRAP_H__
#define __CLUSTERBOOTSTRAP_H__

#define REQUEST "Clusterboot-URL-Request"
#define TIMEOUT_MAX 60
#define RESPONSE_MAXLEN 2048

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netinet/udp.h>


int sock;
struct sockaddr_in destination;
FILE *logfile;
struct timeval timeout;
char response[RESPONSE_MAXLEN];

int clusterboot_socket(void);
void send_broadcast(void);
int get_response(void);
int handle_response(void);
void increase_timeout(void);

#endif
