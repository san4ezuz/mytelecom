#include "connectionFunctions.h"
#include "constants.h"

/* шлем клиенту все, что хранится в dataList,после каждой посланной строчки
 * ожидаем очередного запроса, кроме строчки вида ".", сигнализирующей о конце передачи
 * Если послать не удалось - возвращает -1
 */
int sendResponse(int sock, dataList* dl) {
    int res = 0;
    int i = 0;
    for (i = 0; i < dl->length; ++i) {
        res = sendLine(sock,dl->data[i]);
        if (res == -1) {
            return -1;
        }
    }
    return 0;
}


/*используется клиентом. Шлет в общем случае несколько строк запроса из параметра
 * request  и принимает в общем случае несколько строк ответа, которые сохраняет в 
 * параметр response
 */
int sendAndRecv(int sock, dataList* request, dataList* response) {
    int i = 0;
    int res = 0;
    char buf[DEFAULT_BUFLEN];
    for (i = 0; i < request->length; ++i) {
        bzero(buf, DEFAULT_BUFLEN);
        int res = sendLine(sock, request->data[i]);
        if (res <= 0)
            return -1;
        res = recvLine(sock, buf, DEFAULT_BUFLEN);
        if (res <= 0)
            return -1;

        if (strcmp(buf, ".") == 0) { // если сервер говорит об окончании передачи, то шлем ему следующую строку
            continue;
        }
        addToList(response, buf); // сохраняем первую строку ответа и дочитываем остальной
        for (;;) { // если пришла не точка, значит сервер еще будет досылать ответ. Ждем его
            bzero(buf, DEFAULT_BUFLEN);
            res = recvLine(sock, buf, DEFAULT_BUFLEN);
            if (res <= 0)
                return -1;
            // если сервер говорит об окончании передачи, выходим из внутреннего цикла и шлем оставшиеся строки запроса
            if (strcmp(buf, ".") == 0) {
                break;
            }
            addToList(response, buf);
        }
    }
}

void printAddrInfo(struct sockaddr_in* addr) {
    printf("Addr info: %s:%d\n", inet_ntoa(addr->sin_addr), ntohs(addr->sin_port));
}

int sendLine(int sock, char* str) {

    char tempBuf[DEFAULT_BUFLEN];
    strcpy(tempBuf, str);
    char lineDelim[2];
    sprintf(lineDelim, "%c", LINE_DELIM);
    strcat(tempBuf, lineDelim);
    
    int res = send(sock, tempBuf, strlen(tempBuf), 0);
    return res;
}

int recvFix(int sock, char* buf, int size, int flags) {
    if (size == 0)
        return 0;
    return recv(sock, buf, size, flags | MSG_WAITALL);
}

int createClient(char* hostName, char* portName) {
    struct sockaddr_in clientInfo;
    int clientSocket;
    setAddress(hostName, portName, &clientInfo, "tcp");
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        perror("Client: error while using socket");
        exit(1);
    }
    if (connect(clientSocket, (struct sockaddr*) &clientInfo, sizeof (clientInfo))) {
        perror("Client: error while using connect");
        exit(1);
    }
    return clientSocket;
}

void setAddress(char* hostName, char* portName, struct sockaddr_in* info, char* protocol) {
    bzero(info, sizeof (info));
    info->sin_family = AF_INET;
    if (hostName != NULL) {
        if (!inet_aton(hostName, &info->sin_addr)) {
            // mb change
            perror("Unknown host ");
            exit(1);
        }
    } else {
        info->sin_addr.s_addr = htonl(INADDR_ANY);
    }
    char* endptr;
    short port = strtol(portName, &endptr, 0);
    if (*endptr == '\0')
        info->sin_port = htons(port);
    else {
        // mb change
        perror("Unknown port ");
        exit(1);
    }
}

int createServer(char* hostName, char* portName) {
    struct sockaddr_in servInfo;
    int servSocket;
    setAddress(hostName, portName, &servInfo, "tcp");
    servSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (servSocket < 0) {
        perror("Server: Error while using socket");
        exit(1);
    }
    if (bind(servSocket, (struct sockaddr*) &servInfo, sizeof (servInfo))) {
        perror("Server: Error while using bind");
        exit(1);
    }
    if (listen(servSocket, BACKLOG)) {
        perror("Server: Error while using listen");
        exit(1);
    }
    return servSocket;
}

int recvLine(int sock, char* buffer, int buffSize) {
    char* buff = buffer;
    char* currPosPointer;
    int count = 0;
    int strSize = 0;
    char tempBuf[1500];
    char currChar;
    while (--buffSize > 0) {
        if (--count <= 0) {
            count = recvFix(sock, tempBuf, strSize, 0);
            if (count != strSize) {
                printf("Very strange error... readLine\n");
                return -1;
            }
            count = recv(sock, tempBuf, sizeof (tempBuf), MSG_PEEK);

            if (count <= 0)
                return count;
            currPosPointer = tempBuf;
            strSize = 0;
        }
        currChar = *currPosPointer++;
        *buffer++ = currChar;
        ++strSize;
        if (currChar == LINE_DELIM) {
            *(buffer - 1) = '\0';
            count = recvFix(sock, tempBuf, strSize, 0);
            if (count != strSize) {
                printf("Very strange error... readLine\n");
                return -1;
            }
            return buffer - buff;
        }

    }
    return -1;
}