#include <stdio.h>
#include <stdlib.h>
#include "connectionFunctions.h"
#include "constants.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("You should use at least key S or C");
        exit(0);
    }
    // если хотят запустить сервер
    if (strncmp(argv[1], "S", 1) == 0) {
        //если не задан порт
        if (argc == 2) {
            printf("Using default port: %s\n", DEFAULT_PORT);
            serverMain(DEFAULT_PORT);
        } else {
            serverMain(argv[2]);
        }
        // если хотят запустить клиент
    } else if (strncmp(argv[1], "C", 1) == 0) {
        // если не задано ни адреса ни порта
        if (argc == 2) {
            printf("Using defalut ip and port: %s: %s\n", DEFAULT_SERV_ADDR, DEFAULT_PORT);
            clientMain(DEFAULT_SERV_ADDR, DEFAULT_PORT);
        } else if (argc == 3) {
            // если задан только адрес
            printf("Using defalut port: %s:\n", DEFAULT_PORT);
            clientMain(argv[2], DEFAULT_PORT);
        } else {
            clientMain(argv[2], argv[3]);
        }
    } else {
        // если неизвестный ключ запуска
        printf("Illegal first argument '%s'. First argument must be 'S' or 'C'\n", argv[1]);
        exit(0);
    }
    return 0;
}

