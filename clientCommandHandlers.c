#include "clientCommandHandlers.h"
#include "constants.h"
#include "stdio.h"
#include "commands.h"
extern void printMainMenu();
int checkCommand(command* c) {
    printf("Command is %s\n",c->arguments[0]);
    if (strcmp(c->arguments[0], LOGIN_CMD) == 0) {
        return c->argumentsSize == 3 ? 1 : 0;
    }
    if (strcmp(c->arguments[0], REG_CMD) == 0) {
        return c->argumentsSize == 3 ? 1 : 0;
    }
    if (strcmp(c->arguments[0], INFO_CMD) == 0) {
        if (c->argumentsSize == 1)
            return 1;
        if (c->argumentsSize != 2 || atoi(c->arguments[1]) <= 0) // если нет аргумента, или аргумент не число
            return 0;
        return 1;
    }
    if (strcmp(c->arguments[0], COUNT_CMD) == 0) {
        if (c->argumentsSize != 1)
            return 0;
        return 1;
    }
    if (strcmp(c->arguments[0], BODY_CMD) == 0) {
        if (c->argumentsSize != 2)
            return 0;
        if (atoi(c->arguments[1]) <= 0)
            return 0;
        return 1;
    }
    if (strcmp(c->arguments[0], DEL_CMD) == 0) {
        if (c->argumentsSize != 2)
            return 0;
        if (atoi(c->arguments[1]) <= 0)
            return 0;
        return 1;
    }
    if (strcmp(c->arguments[0], FIRST_LINES_CMD) == 0) {
        if (c->argumentsSize != 3)
            return 0;
        if (atoi(c->arguments[1]) <= 0)
            return 0;
        if (atoi(c->arguments[2]) <= 0)
            return 0;
        return 1;
    }
    if (strcmp(c->arguments[0], MENU_CMD) == 0) {
        return c->argumentsSize == 1 ? 1 : 0;
    }
    if (strcmp(c->arguments[0], EXIT_CMD) == 0) {
        return c->argumentsSize == 1 ? 1 : 0;
    }
    if (strcmp(c->arguments[0], LOGOUT_CMD) == 0) {
        return c->argumentsSize == 1 ? 1 : 0;
    }
    if (strcmp(c->arguments[0], SEND_CMD) == 0) {
        return c->argumentsSize == 2 ? 1 : 0;
    }
    return 0;
}

/* Подготавливает запрос к отправке или в простейшем случае сам выполняет его
 * возвращает -1 в случае ошибки, 1 - если сам выполнил и 0 если запрос подготовлен
 * и его надо отослать
 */
int prepareRequest(command* c, dataList* request) {
    if (!checkCommand(c)) {
        printf("\tIncorrect command or arguments\n");
        return -1;
    }
    if (strcmp(c->arguments[0], MENU_CMD) == 0) { // если просят показать меню, просто показываем и ничего серверу не шлем
        printMainMenu();
        return 1;
    }
    if (strcmp(c->arguments[0], EXIT_CMD) == 0) { //
        printf("Sending last message to server\n");
    }
    addToList(request, c->commandLine);
    if (strcmp(c->arguments[0], SEND_CMD) == 0) { // если хотят отправить письмо, то сначала считываем его тело из консоли
        readLetter(request);
    }
    return 0;
}

int readLetter(dataList* dl) {
    printf("Input letter. Empty string to finish\n");
    char line[DEFAULT_BUFLEN];
    for (;;) {
        bzero(line, DEFAULT_BUFLEN);
        fgets(line, DEFAULT_BUFLEN, stdin);
        line[strlen(line) - 1] = '\0';
        if (strlen(line) == 0) {
            addToList(dl, ".");
            break;
        }
        addToList(dl, line);
    }
    return 1;
}
