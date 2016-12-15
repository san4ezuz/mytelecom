#ifndef COMMANDS_H
#define	COMMANDS_H

// команды, передающиеся клиентом серверу (если больше 8 символов -> поправить функцию printMainMenu
#define LOGIN_CMD "login"
#define REG_CMD "reg"
#define INFO_CMD "list"
#define COUNT_CMD "stat"
#define BODY_CMD "retr"
#define DEL_CMD "delete"
#define FIRST_LINES_CMD "top"
#define MENU_CMD "menu"
#define EXIT_CMD "exit"
#define LOGOUT_CMD "logout"
#define SEND_CMD "send"

// команды консоли сервера (если больше 8 символов -> поправить функцию printServerMenu
#define SHOW_SERV_CMD "show"
#define KICK_SERV_CMD "kill"
#define EXIT_SERV_CMD "exit"
#define MENU_SERV_CMD "menu"

// команды, которые шлет сервер клиенту
#define BYE_STRING "+OK: bye-bye"

#endif	/* COMMANDS_H */

