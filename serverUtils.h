#ifndef SERVERUTILS_H
#define	SERVERUTILS_H

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


void userLoggin(user* client, char* userName);
void userLoggout(user* client);
int addUsr(char* userName, char*pass);
int isUsrRegistred(char* userName);
int checkPass(char* userName, char*password);

void closeAllSockets();
int getClientByIpAndPort(char* ip, int port);
int isUserPresentNow(char* userName);

#endif	/* SERVERUTILS_H */

