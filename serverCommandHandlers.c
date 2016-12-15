#include "serverCommandHandlers.h"
#include "myTypes.h"
#include "connectionFunctions.h"
#include "serverUtils.h"
#include "fileSystemFunctions.h"
#include "commands.h"
extern pthread_mutex_t userlist_lock;
extern user userlist[MAX_USERS];
// выбирает обработчик команды, заданной в параметре command
servCmdHndlrPtr selectHandler(char* command) {

    if (strcmp(command, LOGIN_CMD) == 0)
        return loginCmdHandler;
    if (strcmp(command, REG_CMD) == 0)
        return regCmdHandler;
    if (strcmp(command, INFO_CMD) == 0)
        return infoCmdHandler;
    if (strcmp(command, COUNT_CMD) == 0)
        return countCmdHandler;
    if (strcmp(command, BODY_CMD) == 0)
        return bodyCmdHandler;
    if (strcmp(command, DEL_CMD) == 0)
        return delCmdHandler;
    if (strcmp(command, FIRST_LINES_CMD) == 0)
        return firstLinesCmdHandler;
    if (strcmp(command, EXIT_CMD) == 0)
        return exitCmdHandler;
    if (strcmp(command, LOGOUT_CMD) == 0)
        return logoutCmdHandler;
    if (strcmp(command, SEND_CMD) == 0)
        return sendCmdHandler;
    return defaultCmdHandler;
}
/* обработчики команд. Список команд в файле commands.h
 * Выполняют обработку команды заданной параметром request и отсылают результат пользователю
 * данные о котором хранит параметр client. Параметр response используется для хранения
 * промежуточного результата и номера сообщения для посылки клиенту
 * Возвращают -1 если связь с клиентом утеряна
 */
int loginCmdHandler(user *client, command* request, dataList* response) {
    pthread_mutex_lock(&userlist_lock);
    int alreadyWorking = isUserPresentNow(request->arguments[1]);
    pthread_mutex_unlock(&userlist_lock);
    if (alreadyWorking == 1) {
        addToList(response, "-ERR: user with this name is working now");
    } else if (client->isLogged == 1) {
        addToList(response, "-ERR: logout first");
    } else if (checkPass(request->arguments[1], request->arguments[2])) {
        addToList(response, "-ERR: wrong user name or password");
    } else {
        addToList(response, "+OK: login successfull");
        userLoggin(client, request->arguments[1]);
    }
    addToList(response, ".");
    return sendResponse(client->sock, response);
}
int regCmdHandler(user *client, command* request, dataList* response) {
    if (client->isLogged == 1) {
        addToList(response, "-ERR: logout first");
    } else if (addUsr(request->arguments[1], request->arguments[2]) != 0) {
        addToList(response, "-ERR: user with this name already presents");
    } else {
        addToList(response, "+OK: register successfull");
        userLoggin(client, request->arguments[1]);
    }
    addToList(response, ".");
    return sendResponse(client->sock, response);
}
int countCmdHandler(user *client, command* request, dataList* response) {
    if (client->isLogged == 0) {
        addToList(response, "-ERR: login first");
    } else {
        int lettersCount = getLettersCount(client->name);
        char tmp[DEFAULT_BUFLEN];
        lettersCount == -1 ? sprintf(tmp, "-ERR: can't read directory") : sprintf(tmp, "+OK: %d", lettersCount);
        addToList(response, tmp);
    }
    addToList(response, ".");
    return sendResponse(client->sock, response);
}
int infoCmdHandler(user* client, command* request, dataList* response) {
    if (client->isLogged == 0) {
        addToList(response, "-ERR: login first");
    } else {
        //проверяем не пуста ли папка
        int numLetters = getLettersCount(client->name);
        if (numLetters == 0) {
            addToList(response, "+OK: directory is empty");
        } else {
            // если просят информацию обо всех письмах
            if (request->argumentsSize == 1) {
                int i;
                for (i = 1; i <= numLetters; ++i) { //TODO CHECKKKKK!
                    getInfoAboutLetter(client, response, i);
                    printDataList(response);
                    if (sendResponse(client->sock, response) == -1) {
                        return -1;
                    }
                    clearList(response);
                }
            } else { // если просят информацию только об одном письме
                // проверяем корректный ли номер письма
                if (atoi(request->arguments[1]) > numLetters || atoi(request->arguments[1]) <= 0) { // вторая проверка на всякий случай
                    addToList(response, "-ERR: no letter with this number");
                } else
                    getInfoAboutLetter(client, response, atoi(request->arguments[1]));
            }
        }
    }
    addToList(response, ".");
    return sendResponse(client->sock, response);
}
int bodyCmdHandler(user *client, command* request, dataList * response) {
    if (client->isLogged == 0) {
        addToList(response, "-ERR: login first");
    } else {
        // проверяем есть ли вобще такое письмо
        int letterNumber = atoi(request->arguments[1]);
        int lettersCount = getLettersCount(client->name);
        if (letterNumber > lettersCount) { // пусть письма нумеруются с 1, главное не забыть потом
            addToList(response, "-ERR: there is no letter with this number");
        } else {
            // если такое письмо есть, надо его послать
            getLetterBody(client, letterNumber, response);
        }
    }
    addToList(response, ".");
    return sendResponse(client->sock, response);
}
int delCmdHandler(user *client, command* request, dataList * response) {
    if (client->isLogged == 0) {
        addToList(response, "-ERR: login first");
    } else {
        // проверяем есть ли вобще такое письмо
        int letterNumber = atoi(request->arguments[1]);
        int lettersCount = getLettersCount(client->name);
        if (letterNumber > lettersCount) { // пусть письма нумеруются с 1, главное не забыть потом
            addToList(response, "-ERR: there is no letter with this number");
        } else { // пытаемся удалить писмьо
            int res = deleteLetter(client->name, letterNumber);
            addToList(response, res == 0 ? "+OK: letter deleted" : "-ERR: some error while deleting letter");
        }
    }
    addToList(response, ".");
    return sendResponse(client->sock, response);
}
int firstLinesCmdHandler(user *client, command* request, dataList * response) {
    if (client->isLogged == 0) { // если не зарегистрирован, шлем ошибку
        addToList(response, "-ERR: login first");
    } else {
        // проверяем есть ли вобще такое письмо
        int letterNumber = atoi(request->arguments[1]);
        int lettersCount = getLettersCount(client->name);
        if (letterNumber > lettersCount) { // пусть письма нумеруются с 1, главное не забыть потом
            addToList(response, "-ERR: there is no letter with this number");
        } else {
            // если есть, надо отдать первые M строк этого письма
            getLetterHead(client, letterNumber, atoi(request->arguments[2]), response);
        }
    }
    addToList(response, ".");
    return sendResponse(client->sock, response);
}
int defaultCmdHandler(user *client, command* request, dataList * response) {
    addToList(response, "-ERR: unrecognized command");
    addToList(response, ".");
    return sendResponse(client->sock, response);

}
int exitCmdHandler(user *client, command* request, dataList * response) {
    addToList(response,BYE_STRING);
    addToList(response, ".");
    sendResponse(client->sock, response);
    return -1;
}
int logoutCmdHandler(user *client, command* request, dataList * response) {
    if (client->isLogged == 0) {
        addToList(response, "-ERR: login first");
    } else {
        userLoggout(client);
        addToList(response, "+OK: you are not logged now");
    }
    addToList(response, ".");
    return sendResponse(client->sock, response);
}
int sendCmdHandler(user *client, command* request, dataList * response) {
    if (client->isLogged == 0) { // если не зарегистрирован, шлем ошибку
        addToList(response, "-ERR: login first");
        addToList(response, ".");
        return sendResponse(client->sock, response);
    }
    // проверяем зарегистрирован ли получател
    int res = isUsrRegistred(request->arguments[1]);
    if (res != 0) {// если получатель не зарегестрирован у нас
        addToList(response, "-ERR: no such recipient");
        addToList(response, ".");
        return sendResponse(client->sock, response);
    }
    // шлем подтвержение полученного запроса 
    addToList(response, ".");
    if (sendResponse(client->sock, response) == -1) {
        return -1;
    }
    char buf[DEFAULT_BUFLEN];
    bzero(buf,DEFAULT_BUFLEN);
    dataList tmpLetter;
    initDataList(&tmpLetter);
    // принимаем тело письма
    for (;;) {
        clearList(response);
        bzero(buf,DEFAULT_BUFLEN);
        // получаем очередную строку данных
        if (recvLine(client->sock,buf,DEFAULT_BUFLEN) == -1) {
            return -1;
        } else {
            if (strcmp(buf, ".") == 0) { // если точка значит письмо кончилось
                break;
            }
            addToList(response, "."); // и шлем подтверждение что мы приняли очередную строку
            if (sendResponse(client->sock, response) == -1) {
                return -1;
            }
            addToList(&tmpLetter, buf); // сохраняем строку во временное хранилище
        }
    }
    res = saveLetter(client->name, request->arguments[1], &tmpLetter); // сохраняем пиьсмо уже на диск
    res == -1 ? addToList(response, "-ERR: can't save letter") : addToList(response, "+OK: letter saved");
    addToList(response, ".");
    return sendResponse(client->sock, response);
}


