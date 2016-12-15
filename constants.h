#ifndef CONSTANTS_H
#define	CONSTANTS_H

#include "stddef.h"
#include "string.h"
#define LINE_DELIM '\n'

#define DEFAULT_PORT "7500"
#define DEFAULT_SERV_ADDR "127.0.0.1"

#define DEFAULT_BUFLEN 512
#define MAX_ARGS 4 // максимальное количество аргументов в команде
#define MAX_ARG_SIZE 256 // максимальная длинна одного аргумента команды
#define NUM_MSG_LENGTH 7 // максимальное количество разрядов в номере сообщения
#define REQUEST_TIMEOUT 1000 // максимальное время между запросами от клиента. (после этого кикаем)
#define REQUEST_TRIES 4 // сколько раз клиент пытается получить ответ на свой запрос. (после каждой попытки таймаут увеличивается на 1 секуду)
#define BACKLOG 5

#define MAX_USERS 2
#define USERS_FILE "mail/users.txt"
#define MAX_LETTERS_STRINGS 40

#define HEADERS_DELIM "----"
#define HEADERS_DELIM_LENGTH 4

#endif	/* CONSTANTS_H */

