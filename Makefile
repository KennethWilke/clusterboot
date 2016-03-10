all: clusterbootstrap

clusterbootstrap: clusterbootstrap.c clusterbootstrap.h
	gcc -o $@ -Wall -Werror clusterbootstrap.c
