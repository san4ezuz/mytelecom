#ifndef CONNECTIONFUNCTIONS_H
#define	CONNECTIONFUNCTIONS_H

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/uio.h>

#include "myTypes.h"

int createClient(char* hostName, char* portName);
int createServer(char* hostName, char* portName);


void setAddress(char* hostName, char* portName, struct sockaddr_in* info, char* protocol);

int sendResponse(int sock, dataList* dl);
int sendLine(int sock, char* line);
int recvLine(int sock, char* buffer, int buffSize);

int sendAndRecv(int sock, dataList* request, dataList* response);

void printAddrInfo(struct sockaddr_in* addr);
int recvFix(int sock, char* buf, int size, int flags);
#endif	/* CONNECTIONFUNCTIONS_H */

