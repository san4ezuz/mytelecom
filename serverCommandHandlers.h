#ifndef SERVERCOMMANDHANDLERS_H
#define	SERVERCOMMANDHANDLERS_H

#include "myTypes.h"
#include "constants.h"
servCmdHndlrPtr selectHandler(char* command);

int loginCmdHandler(user *client, command* request, dataList* response);
int regCmdHandler(user *client, command* request, dataList* response);
int countCmdHandler(user *client, command* request, dataList* response);
int infoCmdHandler(user *client, command* request, dataList* response);
int bodyCmdHandler(user *client, command* request, dataList* response);
int delCmdHandler(user *client, command* request, dataList* response);
int firstLinesCmdHandler(user *client, command* request, dataList* response);
int defaultCmdHandler(user *client, command* request, dataList* response);
int exitCmdHandler(user *client, command* request, dataList* response);
int logoutCmdHandler(user *client, command* request, dataList* response);
int sendCmdHandler(user *client, command* request, dataList* response);
#endif	/* SERVERCOMMANDHANDLERS_H */

