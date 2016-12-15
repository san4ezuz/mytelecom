#ifndef MYTYPES_H
#define	MYTYPES_H

#include "constants.h"

typedef struct {
    char name[DEFAULT_BUFLEN];
    int sock;
    int isLogged;
    int index;
    char *ipAddr[DEFAULT_BUFLEN];
    int port;
    char* serverIP[DEFAULT_BUFLEN];
    int serverPort;
} user;

typedef struct {
    int length;
    char data[MAX_LETTERS_STRINGS][DEFAULT_BUFLEN];
} dataList;

typedef struct {
    char commandLine[DEFAULT_BUFLEN];
    char** arguments;
    int argumentsSize;
} command;

typedef int (*servCmdHndlrPtr)(user* client, command* cmd, dataList* response);
typedef int (*consoleCmdHndlrPtr)(command* cmd);
typedef int (*cmdHndlrPtr)(command* c, dataList* dl);

void addToList(dataList* dl, char* string);
void printDataList(dataList* dl);
void clearList(dataList* dl);

void initCommand(command* c);
void clearCommand(command* c);
void parseCommand(command* c);
#endif	/* MYTYPES_H */

