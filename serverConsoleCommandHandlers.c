#include "serverConsoleCommandHandlers.h"
#include "myTypes.h"
#include "commands.h"
extern user userlist[];
extern pthread_mutex_t userlist_lock;
extern pthread_mutex_t users_file_lock;

int consoleShow(command* consoleCommand) {
    printf("\tConnected users are:\n");
    int i = 0;
    user u;
    for (i = 0; i < MAX_USERS; ++i) {
        pthread_mutex_lock(&userlist_lock);
        u = userlist[i];
        pthread_mutex_unlock(&userlist_lock);
        if (u.sock == -1) {
            continue;
        }
        printf("User # %d\n", u.index + 1);
        printf("IP:PORT = %s:%d\n", u.ipAddr, u.port);
        u.isLogged == 1 ? printf("Name: %s\n", u.name) : printf("Not authorized\n");
    }
    return 0;
}

int consoleKill(command* consoleCommand) {
    if (consoleCommand->argumentsSize != 2) {
        printf("Need 1 argument\n");
        return 0;
    }
    int sockIndex = atoi(consoleCommand->arguments[1]);
    if (sockIndex <= 0 || sockIndex >= MAX_USERS) {
        printf("Wrong index\n");
        return 0;
    }
    closeClientByIndex(sockIndex - 1);
    return 0;
}

int consoleExit(command* consoleCommand) {
    return 1;
}

int consoleMenu(command* consoleCommand) {
    printServerMenu();
    return 0;
}

int consoleDefault(command* consoleCommand) {
    printf("Unknown command\n");
    return 0;
}

int closeClientByIndex(int index) {
     pthread_mutex_lock(&userlist_lock);
    if(userlist[index].sock == -1) {
        pthread_mutex_unlock(&userlist_lock);    
        printf("No user with this index\n");
        return -1;
    }
    close(userlist[index].sock);
    userlist[index].sock = -1; // показываем, что место свободно
    bzero(userlist[index].ipAddr, sizeof (userlist[index].ipAddr));
    userlist[index].port = -1;
    userlist[index].index = -1;
    pthread_mutex_unlock(&userlist_lock);
}

void printServerMenu() {
    printf("Server's menu: \n \n");
    printf("|=====Commands:==============|===========================|\n");
    printf("| show user list    -> %8s     \n",SHOW_SERV_CMD);
    printf("| kill user         -> %8s <N> \n",KICK_SERV_CMD);
    printf("| finish work       -> %8s     \n",EXIT_SERV_CMD);
    printf("| show menu         -> %8s     \n",MENU_SERV_CMD);
    printf("|============================|=========================================|\n");
}

consoleCmdHndlrPtr selectConsoleCommand(char* command) {
    if (strcmp(command, SHOW_SERV_CMD) == 0)
        return consoleShow;
    if (strcmp(command, KICK_SERV_CMD) == 0)
        return consoleKill;
    if (strcmp(command, EXIT_SERV_CMD) == 0)
        return consoleExit;
    if (strcmp(command, MENU_SERV_CMD) == 0)
        return consoleMenu;
    return consoleDefault;
}
