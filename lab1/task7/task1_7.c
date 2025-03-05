#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>

typedef enum error {
    OK,
    NULL_POINTER_ERROR,
    INPUT_ERROR,
    MEMORY_ERROR,
    NUMBER_OVERFLOW,
    FILE_ERROR,
    INVALID_ACCOUNT,
} error;

typedef struct myFile {
    char *fileName;
    unsigned char fileType;
    int fileSize;
    int fileMode;
    int filePermission;
    unsigned long inode;
} myFile;

error get_catalog(myFile **files, char *catalog, size_t *catalogSize, size_t *actualSize);

void free_catalog(myFile *files, size_t actualSize);

int main(int args, char *argv[]) {
    myFile *files = NULL;
    size_t catalogSize = 256;
    size_t actualSize = 0;
    error err;

    if (args < 2) {
        printf("INPUT_ERROR. Please enter at least one directory name.\n");
        return INPUT_ERROR;
    }

    for (int i = 1; i < args; i++) {
        if (argv[i] == NULL) {
            printf("MEMORY_ERROR.\n");
            return MEMORY_ERROR;
        }
        printf("===================================================="
               "========================================\n");
        printf("Directory: %s\n", argv[i]);
        err = get_catalog(&files, argv[i], &catalogSize, &actualSize);

        if (err != OK) {
            printf(err == INPUT_ERROR ? "INPUT_ERROR. A directory with this name was not found.\n" : "MEMORY_ERROR\n");
            printf("========================================================="
                   "===================================\n");
            free_catalog(files, actualSize);
            return err;
        }

        for (size_t j = 0; j < actualSize; j++) {
            printf("%-25s | Size: %-8d | Inode: %-8lu | File mode: %-8d\n",
                   files[j].fileName,
                   files[j].fileSize,
                   files[j].inode,
                   files[j].fileMode);
        }

        free_catalog(files, actualSize);
    }
    printf("============================================="
           "===============================================\n");
    return OK;
}

void free_catalog(myFile *files, size_t actualSize) {
    if (files == NULL) {
        return;
    }

    for (size_t i = 0; i < actualSize; i++) {
        free(files[i].fileName);
    }

    free(files);
}

error get_catalog(myFile **files, char *catalog, size_t *catalogSize, size_t *actualSize) {
    struct dirent *dir;
    struct stat st;
    char path[PATH_MAX];
    size_t size = 0;

    DIR *dir_p = opendir(catalog);
    if (dir_p == NULL) {
        return INPUT_ERROR;
    }
    // Выделяем память под массив файлов
    myFile *data = (myFile *) malloc(sizeof(myFile) * (*catalogSize));
    if (data == NULL) {
        closedir(dir_p);
        return MEMORY_ERROR;
    }
    // Главный цикл
    while ((dir = readdir(dir_p)) != NULL) {
        // Не выводим каталоги . и ..
        if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) {
            continue;
        }

        // Для stat создаем полный путь директория + имя файла
        snprintf(path, sizeof(path), "%s/%s", catalog, dir->d_name);

        if (stat(path, &st) != 0) {
            free_catalog(data, size);
            closedir(dir_p);
            return INPUT_ERROR;
        }

        // Проверка на переполнение массива файлов
        if (size == *catalogSize) {
            *catalogSize *= 2;
            myFile *buffer = (myFile *) realloc(data, sizeof(myFile) * (*catalogSize));
            if (buffer == NULL) {
                free_catalog(data, size);
                closedir(dir_p);
                return MEMORY_ERROR;
            }
            data = buffer;
        }

        //Заполняем поля
        data[size].fileName = strdup(dir->d_name);
        if (data[size].fileName == NULL) {
            free_catalog(data, size);
            closedir(dir_p);
            return MEMORY_ERROR;
        }

        data[size].fileType = dir->d_type;
        data[size].fileSize = st.st_size;
        data[size].fileMode = st.st_mode;
        data[size].inode = st.st_ino;

        size++;
    }

    closedir(dir_p);

    *files = data;
    *actualSize = size;
    return OK;
}
