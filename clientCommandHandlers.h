#ifndef CLIENTCOMMANDHANDLERS_H
#define	CLIENTCOMMANDHANDLERS_H
#include "myTypes.h"
int checkCommand(command* c);
int prepareRequest(command* c, dataList* request);
int readLetter(dataList* dl);
#endif	/* CLIENTCOMMANDHANDLERS_H */

