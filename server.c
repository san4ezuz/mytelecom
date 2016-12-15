#include "constants.h"
#include "myTypes.h"
#include "serverCommandHandlers.h"
#include "stdio.h"
#include "serverConsoleCommandHandlers.h"
#include "connectionFunctions.h"
#include "serverUtils.h"
#include "pthread.h"
int initUserList();
int getFreePlace();
int addClient(struct sockaddr_in* clientAddr, int clientLen);

static void *clientHandler(void *args);
static void *consoleHandler(void *args);

user userlist[MAX_USERS];
pthread_mutex_t userlist_lock;
pthread_mutex_t users_file_lock;
pthread_mutex_t file_system_lock;

int serverMain(char* port) {
    pthread_mutex_init(&userlist_lock, NULL);
    pthread_mutex_init(&users_file_lock, NULL);
    pthread_mutex_init(&file_system_lock, NULL);
    initUserList();
    // создаем поток, обрабатывающий консольный ввод
      pthread_t ConsoleThread;
      if (pthread_create(&ConsoleThread, NULL, consoleHandler, NULL) != 0) {
          printf("Can't create console thread\n");
          exit(1);
      }
    // создаем слушающий сокет
    int listenSocket = createServer(NULL, port);
    char buf[DEFAULT_BUFLEN];
    // принимаем подключения
    struct sockaddr_in clientAddr;
    int clientAddrSize = sizeof (clientAddr);
    for (;;) {
        int newClient = accept(listenSocket, (struct sockaddr*) &clientAddr, &clientAddrSize);
        if (newClient < 0) {
            perror("error while using accept"); 
            break;
        }
        int i = getFreePlace();
        if (i == -1) { 
            close(newClient);
            continue;
        }
        pthread_mutex_lock(&userlist_lock);

        userlist[i].sock = newClient; 
        strcpy(userlist[i].ipAddr, inet_ntoa(clientAddr.sin_addr));
        userlist[i].port = ntohs(clientAddr.sin_port);
        userlist[i].index = i;

        pthread_mutex_unlock(&userlist_lock);
        pthread_t tr1;
        if (pthread_create(&tr1, NULL, clientHandler, (void*) i) != 0) {
            printf("cant create clients thread\n");
            exit(0); //ìîæåò íå ñòîèò òàê ðåçêî ïî exit âûõîäèòü?
        }
    }
    return 0;
}

static void *clientHandler(void *args) {
    int index = (int) args;
    pthread_mutex_lock(&userlist_lock);
    user client = userlist[index]; // don't need mutex,?
    pthread_mutex_unlock(&userlist_lock);
   
    // структура для хранения принятой команды
    command cmd;
    initCommand(&cmd);
    // структура для хранения ответа пользователю
    dataList response;
    initDataList(&response);
    // указатель на функцию, обработчик текущей команды
    servCmdHndlrPtr currentCommand;
    // флаг, сигнализирующий о конце работы
    int end = 0;
    do {
        clearCommand(&cmd); // очищаем прошлую команду
        clearList(&response);
        int res = recvLine(client.sock,cmd.commandLine,DEFAULT_BUFLEN);
        if (res == -1) {
            break;
        }
        parseCommand(&cmd); // выделяем аргументы команды 
        currentCommand = selectHandler(cmd.arguments[0]); // получаем указатель на функцию обработчик комады
        if ((*currentCommand)(&client, &cmd, &response) == -1) // выполняем команду
            end = 1; // если команда вернула ненулевой код, значит возникли ошибки при передаче данных
    } while (end == 0);
    // закрываем сокет клиента и сигнализируем что новый клиент может подключится
    closeClientByIndex(index);
    return 0;
}

static void *consoleHandler(void *args) {
    // делаем stdin неблокирующи 
    int flags = fcntl(F_GETFL, 0);
    flags = flags | O_NONBLOCK;
    fcntl(0, F_SETFL, flags);

    // создаем структуру, где будем хранить пользовательский ввод
    command cmd;
    initCommand(&cmd);
    consoleCmdHndlrPtr currentCommand; // указатель на текущую команду
    printServerMenu();
    int end = 0;
    do {
        clearCommand(&cmd); // очищаем прошлую команду
        fgets(cmd.commandLine, DEFAULT_BUFLEN, stdin); // считываем новую команду
        cmd.commandLine[strlen(cmd.commandLine) - 1] = '\0'; // fgets считывает вместе с концом строки, который нам не нужен
        if (strlen(cmd.commandLine) == 0)
            continue;
        parseCommand(&cmd); // выделяем аргументы команды 
        currentCommand = selectConsoleCommand(cmd.arguments[0]); // получаем указатель на функцию обработчик комады
        if ((*currentCommand)(&cmd) != 0) // выполняем команду
            end = 1;
    } while (end == 0);
    closeAllSockets();
    printf("Bye-bye\n");
    exit(0); 
}

int initUserList() {
    int i = 0;
    for (i = 0; i < MAX_USERS; i++) {
        userlist[i].sock = -1;
        userlist[i].port = -1;
        userlist[i].serverPort = -1;
        userlist[i].index = -1;
        userlist[i].isLogged = 0;
        bzero(userlist[i].ipAddr, sizeof (userlist[i].ipAddr));
        bzero(userlist[i].name, sizeof (userlist[i].name));
        bzero(userlist[i].serverIP, sizeof (userlist[i].ipAddr));
    }
}
// возвращаем первое свободное место в массиве клиентов
int getFreePlace() {
    // mutex вроде не нужен, это ведь только чтение + менять может только этот поток
    int i;
    pthread_mutex_lock(&userlist_lock);
    for (i = 0; i < MAX_USERS; i++) {
        if (userlist[i].sock == -1) {
            pthread_mutex_unlock(&userlist_lock);
            return i;
        }
    }
    pthread_mutex_unlock(&userlist_lock);
    return -1;
}


