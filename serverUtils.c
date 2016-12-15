#include "serverUtils.h"
#include "dirent.h"
#include "constants.h"
#include "myTypes.h"
extern user userlist[];
extern pthread_mutex_t userlist_lock;
extern pthread_mutex_t users_file_lock;

void userLoggin(user *client, char* userName) {
    // сохраняем имя пользователя, и тот факт что он авторизировался
    client->isLogged = 1;
    strcpy(client->name, userName);
    pthread_mutex_lock(&userlist_lock);
    userlist[client->index].isLogged = 1;
    strcpy(userlist[client->index].name, userName);
    pthread_mutex_unlock(&userlist_lock);
}

void userLoggout(user *client) {
    client->isLogged = 0;
    bzero(client->name, DEFAULT_BUFLEN);
    pthread_mutex_lock(&userlist_lock);
    userlist[client->index].isLogged = 0;
    bzero(userlist[client->index].name, DEFAULT_BUFLEN);
    pthread_mutex_unlock(&userlist_lock);
}

int addUsr(char* userName, char*pass) {
    // добавляет пользователя, если такого еще не существует, создает ему папку
    if (isUsrRegistred(userName) == 0) // если такой пользователь уже есть
        return -1;
    pthread_mutex_lock(&users_file_lock);
    FILE *f = fopen(USERS_FILE, "a");
    if (f == NULL) {
        pthread_mutex_unlock(&users_file_lock);
        return -1;
    }
    // дописываем в файл пользователя
    char buf[DEFAULT_BUFLEN];
    bzero(buf, DEFAULT_BUFLEN);
    sprintf(buf, "%s:%s\n", userName, pass);
    fputs(buf, f);
    fclose(f);
    pthread_mutex_unlock(&users_file_lock);
    // создаем папку для почты
    bzero(buf, DEFAULT_BUFLEN);
    sprintf(buf, "mail/%s", userName);
    int res = mkdir(buf, S_IRWXU | S_IRWXG | S_IRWXO);
    return res == 0 ? 0 : -1;
}

int isUsrRegistred(char* userName) {
    pthread_mutex_lock(&users_file_lock);
    FILE *f = fopen(USERS_FILE, "r");
    if (f == NULL) {
        pthread_mutex_unlock(&users_file_lock);
        return -1;
    }
    // читаем файл построчно и сравниваем с текущим именем пользователя
    char buf[DEFAULT_BUFLEN];
    bzero(buf, DEFAULT_BUFLEN);
    char *last;
    while (!feof(f)) {
        bzero(buf, DEFAULT_BUFLEN);
        fgets(buf, DEFAULT_BUFLEN, f);
        // выделяем имя пользователя.
        char* curUserName = strtok_r(buf, ":", &last);
        if (curUserName == NULL) {
            fclose(f);
            pthread_mutex_unlock(&users_file_lock);
            return -1;
        }
        if (strcmp(curUserName, userName) == 0) { // и проверяем совпадает ли оно с переданным
            fclose(f);
            pthread_mutex_unlock(&users_file_lock);
            return 0;
        }
    }
    fclose(f);
    pthread_mutex_unlock(&users_file_lock);
    return -1;
}

int checkPass(char* userName, char*password) {
    pthread_mutex_lock(&users_file_lock);
    FILE *f = fopen(USERS_FILE, "r");
    if (f == NULL) {
        pthread_mutex_unlock(&users_file_lock);
        return -1;
    }
    char curUser[DEFAULT_BUFLEN];
    bzero(curUser, DEFAULT_BUFLEN);
    sprintf(curUser, "%s:%s", userName, password);
    char buf[DEFAULT_BUFLEN];
    // читаем файл построчно и сравниваем с текущим именем пользователя и паролем
    while (!feof(f)) {
        bzero(buf, DEFAULT_BUFLEN);
        fgets(buf, DEFAULT_BUFLEN, f);
        if (strlen(buf) == 0) {
            fclose(f);
            pthread_mutex_unlock(&users_file_lock);
            return -1;
        }
        buf[strlen(buf) - 1] = '\0';
        if (strcmp(curUser, buf) == 0) { // и проверяем совпадает ли оно с переданным
            fclose(f);
            pthread_mutex_unlock(&users_file_lock);
            return 0;
        }
    }
    fclose(f);
    pthread_mutex_unlock(&users_file_lock);
    return -1;
}

void closeAllSockets() {
    int i;
    for (i = 0; i < MAX_USERS; ++i) {
        if (userlist[i].sock == -1)
            continue;
        closeClientByIndex(i);
    }
}

int getClientByIpAndPort(char* ip, int port) {
    int i = 0;
    for (i = 0; i < MAX_USERS; ++i) {
        if (strcmp(userlist[i].ipAddr, ip) == 0 && userlist[i].port == port) {
            return i;
        }
    }
    return -1;
}

int isUserPresentNow(char* userName) {
    int i;
    for(i = 0; i < MAX_USERS; ++i) {
        if(userlist[i].sock == -1 || strlen(userlist[i].name) == 0)
            continue;
        if(strcmp(userlist[i].name,userName) == 0)
            return 1;
    }
    return 0;
}
