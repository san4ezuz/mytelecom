#include "constants.h"
#include "myTypes.h"
#include "clientCommandHandlers.h"
#include "stdio.h"
#include "connectionFunctions.h"
#include "commands.h"
void printMainMenu();
int clientMain(char* addr, char* port) {
    // создаем сокет и подключаем его к серверу
    int sock = createClient(addr, port);
    if(sock == -1) {
        printf("Can't connect to server\n");
        exit(1);
    }
    // переменные, для хранения ввода пользователя
    int res = 0;
    command cmd;
    initCommand(&cmd);
    dataList request, response;
    initDataList(&request);
    initDataList(&response);
    printMainMenu();
    for (;;) {
        clearCommand(&cmd);
        clearList(&request);
        clearList(&response);

        printf("Enter command\n");
        fgets(cmd.commandLine, DEFAULT_BUFLEN, stdin);
        cmd.commandLine[strlen(cmd.commandLine) - 1] = '\0';
        if (strlen(cmd.commandLine) == 0)
            continue;

        parseCommand(&cmd); // разбиваем команду на слова
        res = prepareRequest(&cmd, &request); // проверяем корректность ввода, и если можем, то сами выполняем запрос
        if (res == -1 || res == 1) { // -1 - не корректный ввод, 1 - запрос сами выполнили, не надо просить сервер
            continue;
        }
        res = sendAndRecv(sock, &request, &response);
        if (res == -1) {
            printf("Sorry. Connection is lost\n");
            break;
        }
        printDataList(&response); // пишем ответ сервера
        // проверяем не сообщение ли о завершении
        if (strcmp(response.data[0], BYE_STRING) == 0) {
            break;
        }
    }
    return 0;
}

void printMainMenu() {
    printf("\tMAIL CLIENT \n \n");
    printf("Main menu: \n \n");
    printf("|=====Commands:==============|===========================|\n");
    printf("| register new user -> %8s <NAME> <PASSWORD>    \n",REG_CMD);
    printf("| login             -> %8s <NAME> <PASSWORD>    \n",LOGIN_CMD);
    printf("| send letter       -> %8s <RECIPIENT>          \n",SEND_CMD);
    printf("| number of letters -> %8s                      \n",COUNT_CMD);
    printf("| info about letter -> %8s [<N>]                \n",INFO_CMD);
    printf("| body of letter    -> %8s <N>                  \n",BODY_CMD);
    printf("| delete letter     -> %8s <N>                  \n",DEL_CMD);
    printf("| first <M> strings -> %8s <N> <M>              \n",FIRST_LINES_CMD);
    printf("| finish work       -> %8s                      \n",EXIT_CMD);
    printf("| show menu         -> %8s                      \n",MENU_CMD);
    printf("| logout            -> %8s                      \n",LOGOUT_CMD);
    printf("|============================|=========================================|\n");
}
