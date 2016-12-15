#ifndef FILESYSTEMFUNCTIONS_H
#define	FILESYSTEMFUNCTIONS_H

#include "myTypes.h"
#include "constants.h"
#include "stdio.h"

int getLettersCount(char* userName);
void getLetterHead(user* client, int letterNumber, int maxStrings, dataList* result);
void getInfoAboutLetter(user* client, dataList* dl, int letterNumber);
void getLetterBody(user* client, int letterNumber, dataList* dl);
void getWholeLetter(user* client, int letterNumber, dataList* result);

int deleteLetter(char* userName, int letterNumber);
int saveLetter(char* from, char* to, dataList* tmpLetter);

// вспомогательные функции
void writeHeaders(FILE* f, char* from, char* to);
void getCurTime(char* buf);
int getFirstFreeNumber(char* to);
void shiftFiles(char* userName, int letterNumber);

#endif	/* FILESYSTEMFUNCTIONS_H */

