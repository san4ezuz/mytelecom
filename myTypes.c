#include "myTypes.h"

void addToList(dataList* dl, char* string) {
    if (dl->length == MAX_LETTERS_STRINGS) {
        printf("Error: trying to add string to full list\n");
        return;
    }
    strcpy(dl->data[(dl->length)], string);
    ++(dl->length);
}

void clearList(dataList* dl) {
    dl->length = 0;
    int i;
    for (i = 0; i < MAX_LETTERS_STRINGS; ++i) {
        bzero(dl->data[i], DEFAULT_BUFLEN); // можно оптимизировать
    }
}

void printDataList(dataList* dl) {
    int i;
    for (i = 0; i < dl->length; ++i) {
        printf("\t%s\n", dl->data[i]);
    }
}

void initCommand(command* c) {
    //   printf("Inside init command\n");
    bzero(c->commandLine, DEFAULT_BUFLEN);
    int i = 0;
    //   printf("\tgoing to malloc %d arguments\n",MAX_ARGS);
    c->arguments = (char**) malloc(MAX_ARGS * sizeof (char*));
    for (i = 0; i < MAX_ARGS; ++i) {
        c->arguments[i] = (char*) malloc(MAX_ARG_SIZE * sizeof (char));
    }
    c->argumentsSize = 0;
    //   printf("Exiting init command\n");
}

void initDataList(dataList* dl) {
    //    printf("Inside init data list\n");
    dl->length = 0;
    int i;
    for (i = 0; i < MAX_LETTERS_STRINGS; ++i) {
        bzero(dl->data[1], DEFAULT_BUFLEN);
    }
}

void clearCommand(command* c) {
    bzero(c->commandLine, DEFAULT_BUFLEN);
    int i;
    for (i = 0; i < c->argumentsSize; ++i) {
        bzero(c->arguments[i], DEFAULT_BUFLEN);
    }
}

void parseCommand(command* c) {
    if (c->commandLine == NULL || strlen(c->commandLine) == 0) {
        printf("Error. Tring to parse empty command\n");
        return;
    }
    c->argumentsSize = 0;
    // о функции strtok_r - http://unixcoding.blogspot.ru/2010/10/blog-post_679.html 
    char *tmpCommand = strdup(c->commandLine);
    char *last;
    char* token = strtok_r(tmpCommand, " ", &last); // считываем саму команду
    strcpy(c->arguments[0], token); // и копируем ее как первый аргумент
    ++c->argumentsSize;
    int i = 0;
    for (i = 1; i < MAX_ARGS; ++i) {
        token = strtok_r(NULL, " ", &last);
        if (token == NULL)
            break;
        strcpy(c->arguments[i], token);
        ++c->argumentsSize;
    }
    free(tmpCommand);
}
