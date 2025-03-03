#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>

typedef enum error {
    OK,
    NULL_POINTER_ERROR,
    INPUT_ERROR,
    MEMORY_ERROR,
    NUMBER_OVERFLOW,
    FILE_ERROR,
    INVALID_ACCOUNT,
} error;

#define MAX_LOGIN_LENGTH 6

typedef struct User {
    char login[7];
    unsigned int password;
    int sanctions;
} User;


void p_menu();

void p_start_message();

void flush_stdin();

// error get_word(char **str);

error get_word_with_len(char *str, size_t len_sym);

error check_correct_login(const char *login);

error check_correct_password(unsigned int password);

error get_login(char *login);

error get_password(unsigned int *password);

error registration_user(User **accounts, size_t *size, size_t *capacity, User user_data);

error get_accounts(User **accounts, size_t *size, size_t *capacity);

error check_input_data(User *accounts, const char *password_str);

error search_account(const User *accounts, size_t size, User *user_data);

error string_to_uint(const char *str, unsigned int *res, int base);

void time_command();

void date_command();

error sanctions(User *accounts, size_t size, const char *username, int limit);

error update_sanctions_in_file(const char *filename, User *accounts, size_t size);

int howmuch_command(const char *date_str, const char *flag, double *result);


int main() {
    char user_answer[5];
    User user = {"", 0, -1};
    User *accounts;

    error err;
    size_t size = 0;
    size_t capacity = 2;
    int connect_flag = 0;
    int exit_flag = 0;

    int user_sanctions;
    char command_ask[10] = "";
    char username[7];
    char admin_code[6];
    char date_str[11];
    char flag_time[3];

    int limit;
    double result;
    int error_code;

    // Получаем аккаунты из файла accounts.txt
    err = get_accounts(&accounts, &size, &capacity);
    if (err != OK) {
        printf("ERROR getting accounts\n");
        return err;
    }

    // Главный цикл программы
    do {
        command_ask[0] = '\0';
        connect_flag = 0;
        // Цикл с логином и регистрацией пользователя
        do {
            p_start_message();

            // Получаем ответ user
            if (scanf("%s", user_answer) != 1) {
                printf("Invalid input. Try again\n");
            }
            flush_stdin();

            // Логин пользователя
            if (strcmp(user_answer, "log") == 0) {
                do {
                    // Получаем логин
                    do {
                        printf("\nPlease enter your login: ");
                        err = get_login(user.login);
                        if (err != OK) {
                            printf("Invalid login\n");
                        }
                    } while (err != OK);

                    // Получаем пароль
                    do {
                        printf("\nPlease enter your password: ");
                        err = get_password(&user.password);
                        if (err != OK) {
                            printf("Invalid password\n");
                        }
                    } while (err != OK);

                    // Проверяем, что такой аккаунт действительно существует в нашей БД
                    if (search_account(accounts, size, &user) == OK) {
                        printf("Account found!\n");
                        connect_flag = 1;
                    } else {
                        printf("Account not found! Try again.\n");
                    }
                } while (connect_flag == 0);
            }

            // Регистрация пользователя
            else if (strcmp(user_answer, "reg") == 0) {
                do {
                    // Получаем логин
                    do {
                        printf("\nPlease enter your login (max 6 symbols): ");
                        err = get_login(user.login);
                        if (err != OK) {
                            printf("Invalid login\n");
                        }
                    } while (err != OK);

                    // Получаем пароль
                    do {
                        printf("\nPlease enter your password: ");
                        err = get_password(&user.password);
                        if (err != OK) {
                            printf("Invalid password\n");
                        }
                    } while (err != OK);

                    // Проверяем, что аккаунта нет в БД
                    if (search_account(accounts, size, &user) == OK) {
                        printf("This login is already used.\n");
                    } else {
                        // Добавляем новый аккаунт в БД и в переменную 'accounts'
                        if (registration_user(&accounts, &size, &capacity, user) != OK) {
                            printf("Registration failed.\n");
                            free(accounts);
                            return INVALID_ACCOUNT;
                        }
                        printf("New account created!\n");
                        connect_flag = 1;
                    }
                } while (connect_flag == 0);
            } else if (strcmp(user_answer, "exit") == 0) {
                exit_flag = 1;
                break;
            } else {
                printf("Invalid command. Try again\n");
            }
        } while (connect_flag == 0);

        if (exit_flag == 1) {
            break;
        }

        user_sanctions = user.sanctions;

        // Цикл работы с командами
        while (strcmp(command_ask, "logout") != 0) {
            if (user_sanctions == 0) {
                break;
            }

            p_menu();
            scanf("%s", command_ask);

            if (strcmp(command_ask, "time") == 0) {
                time_command();
                --user_sanctions;
            } else if (strcmp(command_ask, "date") == 0) {
                date_command();
                --user_sanctions;
            } else if (strcmp(command_ask, "howmuch") == 0) {
                if (scanf("%10s %2s", date_str, flag_time) != 2) {
                    printf("Invalid input. Try again\n");
                }
                flush_stdin();

                error_code = howmuch_command(date_str, flag_time, &result);

                if (error_code == -1) {
                    printf("Invalid date format. Use DD-MM-YYYY\n");
                } else if (error_code == -2) {
                    printf("Invalid date.\n");
                } else if (error_code == -3) {
                    printf("Failed to get current time.\n");
                } else if (error_code == -4) {
                    printf("The given date is in the future.\n");
                } else if (error_code == -5) {
                    printf("Invalid flag. Use -s, -m, -h, -y\n");
                } else {
                    printf("Elapsed time: %.2f\n", result);
                }
                --user_sanctions;

            } else if (strcmp(command_ask, "sanctions") == 0) {
                if (scanf("%s %d", username, &limit) != 2) {
                    printf("Invalid input. Try again\n");
                }
                flush_stdin();

                printf("\nPlease enter admin-code: ");
                scanf("%s", admin_code);
                flush_stdin();

                if (strcmp(admin_code, "12345") != 0) {
                    printf("Sorry, code doesn't correct. Try again\n");
                } else {
                    err = sanctions(accounts, size, username, limit);
                    if (err == INPUT_ERROR) {
                        printf("Invalid sanctions count (count > 0). Try again\n");
                    } else if (err == FILE_ERROR) {
                        printf("Sanctions file error\n");
                        exit_flag = 1;
                        --user_sanctions;
                        break;
                    } else if (err == INVALID_ACCOUNT) {
                        printf("Invalid account\n");
                    } else {
                        printf("Sanctions OK\n");
                    }
                }
                --user_sanctions;
            } else if (strcmp(command_ask, "logout") == 0) {
                printf("Logged out!\n");
            } else {
                printf("Invalid command. Try again\n");
                flush_stdin();
            }
        }
    } while (exit_flag == 0);

    free(accounts);
    return OK;
}


void p_menu() {
    printf("================================================================\n");
    printf("Please enter a command from this list:\n");
    printf("1) 'time'\n");
    printf("2) 'date'\n");
    printf("3) 'howmuch' <DD-MM-YYYY> 'flag' {{'Use -s, -m, -h, -y}}\n");
    printf("4) 'sanctions' <username> <count>\n");
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
    printf("    'exit' - Enter to exit.\n");
    printf("----------------------------------------------------------------\n");
}


// error get_word(char **str) {
//     if (str == NULL) {
//         return NULL_POINTER_ERROR;
//     }
//
//     char *temp_str = (char *) malloc(256 + 1);
//     if (temp_str == NULL) {
//         return MEMORY_ERROR;
//     }
//
//     int size = 0, capacity = 256;
//     char sym;
//
//     while ((sym = getchar()) != EOF) {
//         if (sym == '\n') {
//             break;
//         }
//         if (size >= capacity) {
//             capacity *= 2;
//             char *temp = realloc(temp_str, capacity + 1);
//             if (temp == NULL) {
//                 free(temp_str); // Освобождаем старый блок перед выходом
//                 return MEMORY_ERROR;
//             }
//             temp_str = temp;
//         }
//
//         temp_str[size++] = sym;
//     }
//
//     temp_str[size] = '\0';
//     *str = temp_str;
//
//     return OK;
// }


error get_word_with_len(char *str, size_t len_sym) {
    char temp_str[len_sym];

    if (!str) {
        return NULL_POINTER_ERROR;
    }

    if (fgets(temp_str, sizeof(temp_str), stdin) == NULL) {
        return INPUT_ERROR;
    }

    if (temp_str[strlen(temp_str) - 1] != '\n') {
        flush_stdin();
        return INPUT_ERROR;
    }

    // Убираем \n, если он есть
    temp_str[strcspn(temp_str, "\n")] = '\0';

    strcpy(str, temp_str);

    return OK;
}


void flush_stdin() {
    int c;
    while ((c = fgetc(stdin)) != '\n' && c != EOF);
}

error get_login(char *login) {
    char temp[MAX_LOGIN_LENGTH + 1];

    if (get_word_with_len(temp, MAX_LOGIN_LENGTH + 1) != OK) {
        return INPUT_ERROR;
    }

    if (check_correct_login(temp) != OK) {
        return INPUT_ERROR;
    }

    strcpy(login, temp);

    return OK;
}


error get_password(unsigned int *password) {
    char temp_str[10];
    unsigned int temp_password = 0;

    if (get_word_with_len(temp_str, sizeof(temp_str)) != OK) {
        return INPUT_ERROR;
    }

    error error = string_to_uint(temp_str, &temp_password, 10);
    if (error != OK) {
        return error;
    }

    if ((error = check_correct_password(temp_password)) != OK) {
        return error;
    }

    *password = temp_password;
    return OK;
}

error check_correct_login(const char *login) {
    if (!login || login[0] == '\0') {
        return NULL_POINTER_ERROR;
    }

    size_t len = strlen(login);
    if (len > MAX_LOGIN_LENGTH) {
        return INPUT_ERROR;
    }

    for (size_t i = 0; i < len; i++) {
        if (!isalnum(login[i])) {
            return INPUT_ERROR;
        }
    }
    return OK;
}

error check_correct_password(unsigned int password) {
    if (password > 100000) {
        return INPUT_ERROR;
    }

    return OK;
}


error string_to_uint(const char *str, unsigned int *res, int base) {
    char *end = NULL;

    if (str == NULL || *str == '\0') {
        return INPUT_ERROR;
    }

    // Проверка на незначащие нули
    if (str[0] == '0' && str[1] != '\0') {
        return INPUT_ERROR;
    }

    *res = strtoul(str, &end, base);
    if (*end != '\0') {
        return INPUT_ERROR;
    }

    return OK;
}


error check_input_data(User *accounts, const char *password_str) {
    if (check_correct_login(accounts->login) != OK) {
        return INPUT_ERROR;
    }
    if (string_to_uint(password_str, &accounts->password, 10) != OK) {
        return INPUT_ERROR;
    }
    return OK;
}

error get_accounts(User **accounts, size_t *size, size_t *capacity) {
    FILE *file;
    int args_counter;

    User *data = (User *) malloc(sizeof(User) * (*capacity));
    if (data == NULL) {
        return MEMORY_ERROR;
    }

    if ((file = fopen("accounts.txt", "a+")) == NULL) {
        return FILE_ERROR;
    }

    char password_str[8];

    while ((args_counter = fscanf(file, " %6[^;];%7[^;];%d\n",
                                  data[*size].login,
                                  password_str,
                                  &data[*size].sanctions)) == 3) {
        if (check_input_data(&data[*size], password_str) == INPUT_ERROR) {
            free(data);
            fclose(file);
            return INPUT_ERROR;
        }
        (*size)++;

        //Проверка на переполнение бд
        if (*size == *capacity) {
            *capacity *= 2;
            User *buffer = (User *) realloc(data, sizeof(User) * (*capacity));
            if (buffer == NULL) {
                free(data);
                fclose(file);
                return MEMORY_ERROR;
            }
            data = buffer;
        }
    }

    if (args_counter > 0 && args_counter < 3) {
        free(data);
        fclose(file);
        return INPUT_ERROR;
    }

    fclose(file);
    *accounts = data;
    return OK;
}

error search_account(const User *accounts, const size_t size, User *user_data) {
    //Ищем в бд совпадающие логин и пароль
    for (size_t i = 0; i < size; i++) {
        if (strcmp(accounts[i].login, user_data->login) == 0 && accounts[i].password == user_data->password) {
            user_data->sanctions = accounts[i].sanctions;
            return OK;
        }
    }

    return INVALID_ACCOUNT;
}


error registration_user(User **accounts, size_t *size, size_t *capacity, const User user_data) {
    if (*accounts == NULL) {
        return NULL_POINTER_ERROR;
    }

    // Проверка на переполнение перед добавлением
    if (*size == *capacity) {
        *capacity *= 2;
        User *buffer = (User *) realloc(*accounts, sizeof(User) * (*capacity));
        if (buffer == NULL) {
            return MEMORY_ERROR;
        }
        *accounts = buffer;
    }

    // Добавляем нового пользователя
    (*accounts)[*size] = user_data;
    (*size)++;

    // Добавляем новый аккаунт в бд
    FILE *file = fopen("accounts.txt", "a");
    if (file == NULL) {
        return FILE_ERROR;
    }

    fprintf(file, "%s;%u;%d\n", user_data.login, user_data.password, user_data.sanctions);
    fclose(file);

    return OK;
}


void time_command() {
    const time_t time_value = time(NULL);
    const struct tm tm = *localtime(&time_value);
    printf("Current time: %02d:%02d:%02d\n", tm.tm_hour, tm.tm_min, tm.tm_sec);
}

void date_command() {
    const time_t time_value = time(NULL);
    const struct tm tm = *localtime(&time_value);
    printf("Current date: %04d-%02d-%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}


error update_sanctions_in_file(const char *filename, User *accounts, size_t size) {
    const char *temp_filename = "accounts_temp.txt";
    FILE *temp_file = fopen(temp_filename, "w");
    if (temp_file == NULL) {
        return FILE_ERROR;
    }

    for (size_t i = 0; i < size; i++) {
        if (fprintf(temp_file, "%s;%u;%d\n", accounts[i].login, accounts[i].password, accounts[i].sanctions) < 0) {
            fclose(temp_file);
            remove(temp_filename); // Удаляем временный файл при ошибке
            return FILE_ERROR;
        }
    }

    fclose(temp_file);

    // Заменяем старый файл новым
    if (remove(filename) != 0 || rename(temp_filename, filename) != 0) {
        return FILE_ERROR;
    }

    return OK;
}


error sanctions(User *accounts, const size_t size, const char *username, const int limit) {
    if (limit <= 0) {
        return INPUT_ERROR;
    }
    for (size_t i = 0; i < size; i++) {
        if (strcmp(accounts[i].login, username) == 0) {
            accounts[i].sanctions = limit;
            return update_sanctions_in_file("accounts.txt", accounts, size);
        }
    }

    return INVALID_ACCOUNT;
}


int howmuch_command(const char *date_str, const char *flag, double *result) {
    struct tm input_time = {0};
    time_t current_time, input_time_t;
    double seconds_diff;

    // Парсим дату из строки
    if (sscanf(date_str, "%d-%d-%d", &input_time.tm_mday, &input_time.tm_mon, &input_time.tm_year) != 3) {
        return -1; // Ошибка формата даты
    }

    input_time.tm_year -= 1900;
    input_time.tm_mon -= 1;

    // Преобразуем в timestamp
    input_time_t = mktime(&input_time);
    if (input_time_t == -1) {
        return -2; // Ошибка преобразования даты
    }

    // Получаем текущее время
    current_time = time(NULL);
    if (current_time == -1) {
        return -3; // Ошибка получения текущего времени
    }

    // Вычисляем разницу во времени в секундах
    seconds_diff = difftime(current_time, input_time_t);
    if (seconds_diff < 0) {
        return -4; // Дата в будущем
    }

    // Определяем, в каких единицах вернуть результат
    if (strcmp(flag, "-s") == 0) {
        *result = seconds_diff;
    } else if (strcmp(flag, "-m") == 0) {
        *result = seconds_diff / 60;
    } else if (strcmp(flag, "-h") == 0) {
        *result = seconds_diff / 3600;
    } else if (strcmp(flag, "-y") == 0) {
        *result = seconds_diff / (3600 * 24 * 365.25);
    } else {
        return -5;
    }

    return 0;
}
