#include "fileSystemFunctions.h"
#include "clientCommandHandlers.h"
#include "dirent.h"
#include "stdio.h"
#include "time.h"
#include "pthread.h"
extern pthread_mutex_t file_system_lock;
// возвращает количество писем пользователя. Если не может подсчитать, к примеру нет папки, возвращает -1
int getLettersCount(char* userName) {
    // открываем папку пользователя
    char pathToFolder[DEFAULT_BUFLEN];
    bzero(pathToFolder, DEFAULT_BUFLEN);
    sprintf(pathToFolder, "mail/%s", userName);
    DIR* d = opendir(pathToFolder);
    if (d == NULL) {
        return -1;
    }
    int lettersCount = 0;
    // получаем все записи в папке, и если это текстовый файл, увеличиваем счетчик http://www.ibm.com/developerworks/ru/library/au-unix-readdir/
    struct dirent entry;
    struct dirent* result;
    readdir_r(d, &entry, &result);
    while (result) {
        //получаем инфу о файле( неизвестно есть тако поле на всех системах !!!!!)
        if (entry.d_type == DT_REG) //http://www.delorie.com/gnu/docs/glibc/libc_270.html
            lettersCount++;
        // получаем инфу о следующем файле
        readdir_r(d, &entry, &result);
    }
    closedir(d);
    return lettersCount;
}

// записывает заголовки письма в стурктуру типа dataList
// ничего не проверяет, т.к. вызывающая функция должна была уже это сделать
void getInfoAboutLetter(user* client, dataList* dl, int letterNumber) {
    dataList tempArray;
    clearList(&tempArray);
    // получаем все письмо во временный массив
    getWholeLetter(client, letterNumber, &tempArray);
    // проверяем удалось ли прочитать файл
    if (tempArray.length == 0) {
        addToList(dl, " -ERR: can't read file or file is empty");
        return;
    }
    int i;
    char tb[DEFAULT_BUFLEN];
    bzero(tb, DEFAULT_BUFLEN);
    sprintf(tb, "\tLetter # %d", letterNumber);
    addToList(dl, tb);
    for (i = 0; i < tempArray.length; ++i) {
        // если дошли до конца заголовков то прекращаем(подразумевается что заголовки есть ВСЕГДА!)
        if (strncmp(tempArray.data[i], HEADERS_DELIM, HEADERS_DELIM_LENGTH) == 0) {
            break;
        }
        // иначе копируем в основной результирующий массив
        addToList(dl, tempArray.data[i]);
    }
}
// ничего не проверяет, т.к. вызывающая функция должна была уже это сделать

void getLetterBody(user* client, int letterNumber, dataList* dl) {
    dataList tempArray;
    clearList(&tempArray);
    // получаем все письмо во временный массив
    getWholeLetter(client, letterNumber, &tempArray);
    // проверяем удалось ли прочитать файл
    if (tempArray.length == 0) {
        addToList(dl, " -ERR: can't read file");
        return;
    }
    char tb[DEFAULT_BUFLEN];
    bzero(tb, DEFAULT_BUFLEN);
    sprintf(tb, "\tLetter # %d", letterNumber);
    addToList(dl, tb);
    int i;
    int isHeader = 1; // флаг, показывающий что сейчас читаются заголовки
    for (i = 0; i < tempArray.length; ++i) {
        // если наткнулись на конец заголовков
        if (strncmp(tempArray.data[i], HEADERS_DELIM, HEADERS_DELIM_LENGTH) == 0 && isHeader == 1) {
            isHeader = 0;
            continue;
        }
        if (isHeader == 1)
            continue; //  пропускаем заголовок
        // сюда приходим, еcли читаем само тело письма
        addToList(dl, tempArray.data[i]);
    }
}

//возвращает 0, если удалось удалить или -1 если нет
int deleteLetter(char* userName, int letterNumber) {
    char fileName[DEFAULT_BUFLEN];
    bzero(fileName, DEFAULT_BUFLEN);
    sprintf(fileName, "mail/%s/%d", userName, letterNumber);
    if (remove(fileName)) {
        return -1;
    }
    pthread_mutex_lock(&file_system_lock);
    shiftFiles(userName, letterNumber);
    pthread_mutex_unlock(&file_system_lock);
    return 0;
}

// возвращает файл в виде массива строк, проверок не выполняет!
// если возникла ошибка, ничего не добавляет в массив, а просто прекращает выполнение
void getWholeLetter(user* client, int letterNumber, dataList* result) {
    // пытаемся открыть файл
    char buf[DEFAULT_BUFLEN];
    bzero(buf, DEFAULT_BUFLEN);
    sprintf(buf, "mail/%s/%d", client->name, letterNumber);
    FILE* f = fopen(buf, "r");
    if (f == NULL) {
        return;
    }
    // если файл все же открыли читаем построчно и сохраняем в массив
    while (!feof(f)) {
        bzero(buf, DEFAULT_BUFLEN);
        fgets(buf, DEFAULT_BUFLEN, f);
        buf[strlen(buf) - 1] = '\0'; // а если ничего не прочитали?
        addToList(result, buf);
    }
    fclose(f);
}
// записывает в массив result первые maxStrings ТЕЛА письма
void getLetterHead(user* client, int letterNumber, int maxStrings, dataList* result) {
    dataList tempArray;
    clearList(&tempArray);
    // получаем все письмо во временный массив
    getWholeLetter(client, letterNumber, &tempArray);
    // проверяем удалось ли прочитать файл
    if (tempArray.length == 0) {
        addToList(result, " -ERR: can't read file");
        return;
    }
    int i;
    int curStrings = 0;
    int isHeader = 1; // флаг, показывающий что сейчас читаются заголовки
    for (i = 0; i < tempArray.length; ++i) {
        // если наткнулись на конец заголовков
        if (strncmp(tempArray.data[i], HEADERS_DELIM, HEADERS_DELIM_LENGTH) == 0 && isHeader == 1) {
            isHeader = 0;
            continue;
        }
        if (isHeader == 1)
            continue; //  пропускаем заголовок
        // сюда приходим, еcли читаем само тело письма
        if (curStrings >= maxStrings) {
            break;
        } else {
            addToList(result, tempArray.data[i]);
            curStrings++;
        }
    }
}

int saveLetter(char* from, char* to, dataList* tmpLetter) {
    int i = getFirstFreeNumber(to);
    char fileName[DEFAULT_BUFLEN];
    bzero(fileName, DEFAULT_BUFLEN);
    sprintf(fileName, "mail/%s/%d", to, i);
    FILE* f = fopen(fileName, "w");
    if (f == NULL)
        return -1;
    writeHeaders(f, from, to);
    for (i = 0; i < tmpLetter->length; ++i) {
        fputs(tmpLetter->data[i], f);
        fputs("\n", f);
    }
    fclose(f);
    return 0;
}
void writeHeaders(FILE* f, char* from, char* to) {
    char buf[DEFAULT_BUFLEN];
    bzero(buf, DEFAULT_BUFLEN);

    sprintf(buf, "from=%s\n", from);
    fputs(buf, f);

    bzero(buf, DEFAULT_BUFLEN);
    sprintf(buf, "to=%s\n", to);
    fputs(buf, f);


    bzero(buf, DEFAULT_BUFLEN);
    getCurTime(buf);
    fputs(buf, f);

    bzero(buf, DEFAULT_BUFLEN);
    sprintf(buf, "%s\n", HEADERS_DELIM);
    fputs(buf, f);
}

void getCurTime(char* buf) {
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strcpy(buf, (asctime) (timeinfo));

}

int getFirstFreeNumber(char* to) {
    int i = 1;
    FILE *f;
    char buf[DEFAULT_BUFLEN];
    for (;;) {
        bzero(buf, DEFAULT_BUFLEN);
        sprintf(buf, "mail/%s/%d", to, i);
        f = fopen(buf, "r");
        if (f == NULL)
            return i;
        fclose(f);
        ++i;
    }
}

void shiftFiles(char* userName, int letterNumber) {
    char newFileName[DEFAULT_BUFLEN];
    bzero(newFileName, DEFAULT_BUFLEN);
    char oldFileName[DEFAULT_BUFLEN];
    bzero(oldFileName, DEFAULT_BUFLEN);

    sprintf(newFileName, "mail/%s/%d", userName, letterNumber);
    sprintf(oldFileName, "mail/%s/%d", userName, letterNumber + 1);
    int i = letterNumber + 1;
    while (!rename(oldFileName, newFileName)) {
        ++i;
        bzero(newFileName, DEFAULT_BUFLEN);
        bzero(oldFileName, DEFAULT_BUFLEN);
        strcpy(newFileName, oldFileName);
        sprintf(newFileName, "mail/%s/%d", userName, i);
    }
}
