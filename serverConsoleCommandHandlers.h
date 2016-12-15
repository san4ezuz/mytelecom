
#ifndef SERVERCONSOLECOMMANDHANDLERS_H
#define	SERVERCONSOLECOMMANDHANDLERS_H

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
#include "constants.h"

consoleCmdHndlrPtr selectConsoleCommand(char* command);

int consoleShow(command * consoleCommand);
int consoleKill(command * consoleCommand);
int consoleExit(command * consoleCommand);
int consoleMenu(command * consoleCommand);
int consoleDefault(command * consoleCommand);

int closeClientByIndex(int index);
void printServerMenu();

#endif	/* SERVERCONSOLECOMMANDHANDLERS_H */

