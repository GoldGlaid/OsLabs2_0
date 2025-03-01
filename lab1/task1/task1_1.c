#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef enum error {
    OK,
    NULL_POINTER_ERROR,
    INPUT_ERROR,
    MEMORY_ERROR,
    INVALID_INPUT,
} error;

#define GET_WORD(err, word) {\
    err = getword(&word);\
    if (err != OK) {\
        free(word);\
        if (err == NULL_POINTER_ERROR) { \
            printf("NULL_POINTER_ERROR\n");\
            return NULL_POINTER_ERROR;\
            }\
        if (err == MEMORY_ERROR) {\
            printf("MEMORY_ERROR\n");\
            return MEMORY_ERROR;\
        }\
        if (err == INVALID_INPUT) {\
            printf("INVALID_INPUT\n");\
            return INVALID_INPUT;\
        }\
    }}

typedef struct User {
    char login[7];
    unsigned int password;
    int sanctions;
}User;

void p_menu();

void p_start_message();



error getword(char **str);

error check_correct_login(const char *login);

error check_correct_password(unsigned int password);

error new_registration();

int main() {
    error err;
    char *user_answer = "";
    User user = {'', NULL};
    p_start_message();

    char *login;
    char *password;

    GET_WORD(err, user_answer);

    //Тут контекст разделен?
    //Логин пользователя
    if (strcmp(user_answer, "log") == 0) {
        
        printf("\nPlease enter your login: ");
        while (!user.login) {
            scanf('%s', login);
            user.login = login;
        }
       

        printf("\nPlease enter your password: ");
        GET_WORD(err, password);

    //Регистрация пользователя
    } else if (strcmp(user_answer, "reg") == 0) {
        printf("\nPlease enter your login: ");
        GET_WORD(err, login);

        printf("\nPlease enter your password: ");
        GET_WORD(err, password);

        printf("\nPlease repeat your password: ");
        GET_WORD(err, password);
    } else {
        printf("INVALID_INPUT\n");
        return INVALID_INPUT;
    }

    // switch (flag) {
    //     case 'q':
    //         break;
    //     default:
    //
    // }

    free(user_answer);
    return 0;
}

void p_menu() {
    printf("================================================================\n");
    printf("Please enter a command from this list:\n");
    printf("1) 'time'\n");
    printf("2) 'data'\n");
    printf("3) 'howmatch'\n");
    printf("4) 'sanctions_username'\n");
    printf("5) 'logout'\n");
    printf("================================================================\n");
}

void p_start_message() {
    printf("----------------------------------------------------------------\n");
    printf("Hi! \n");
    printf("Log in into yours account.\n");
    printf("New here? Create an account :3\n");
    printf("    'log' - Enter to log in to your account.\n");
    printf("    'reg' - Enter to create a new account.\n");
    printf("----------------------------------------------------------------\n");
}


error getword(char **str) {
    char sym;
    char *temp_str;
    char *temp;
    int capacity = 256;
    int size = 0;

    if (*str == NULL) {
        return NULL_POINTER_ERROR;
    }

    temp_str = (char *) malloc(capacity * sizeof(char) + 1);
    if (temp_str == NULL) {
        return MEMORY_ERROR;
    }

    while ((sym = getchar()) != EOF) {
        if (sym == '\n') {
            temp_str[size] = '\0';
            *str = temp_str;
            return OK;
        }
        if (sym == ' ') {
            *str = temp_str;
            return INVALID_INPUT;
        }

        if (size == capacity) {
            capacity *= 2;
            temp = realloc(temp_str, capacity * sizeof(char) + 1);
            if (temp == NULL) {
                return MEMORY_ERROR;
            }
            temp_str = temp;
        }
        temp_str[size++] = sym;
    }
    return OK;
}


error new_registration() {
    char *login;
    char *password;


    return OK;
}
