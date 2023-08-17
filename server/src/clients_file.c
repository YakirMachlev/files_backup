#include "clients_file.h"

#define MAX_LINE_LENGTH (NAME_MAX_LENGTH + PASSWORD_MAX_LENGTH + 2)

static pthread_mutex_t file_lock = PTHREAD_MUTEX_INITIALIZER;

uint16_t create_files_list(char *files_list, DIR *folder)
{
    struct dirent *result;
    char *start;
    uint8_t file_name_length;
    start = files_list;

    while ((result = readdir(folder)))
    {
        if (strcmp(result->d_name, ".") && strcmp(result->d_name, ".."))
        {
            file_name_length = strlen(result->d_name);
            *(files_list++) = file_name_length;
            strncpy(files_list, result->d_name, file_name_length);
            files_list += file_name_length;
        }
    }
    return files_list - start;
}

uint8_t count_files_in_directory(DIR *folder)
{
    struct dirent *result;
    uint8_t count;

    count = 0;
    while ((result = readdir(folder)))
    {
        if (strcmp(result->d_name, ".") && strcmp(result->d_name, ".."))
        {
            ++count;
        }
    }
    return count;
}

DIR *open_folder(char *path)
{
    DIR *folder;
    struct dirent *result;
    struct stat sb;

    folder = NULL;
    if (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode))
    {
        folder = opendir(path);
        if (access(path, F_OK) != -1)
        {
            if (folder)
            {
                return folder;
            }
            else
            {
                perror("Could not open the directory");
                exit(1);
            }
        }
        else
        {
            printf("The %s it cannot be opened or is not a directory\n", path);
            exit(1);
        }
    }
    return NULL;
}

bool check_name_validity(char *name)
{
    uint8_t offset;
    for (offset = 0; offset < NAME_MAX_LENGTH && *name; offset++)
    {
        if (!isalnum(*(name++)))
        {
            return false;
        }
    }
    return true;
}

void insert_client_to_file(char *name, char *password)
{
    FILE *fp;

    pthread_mutex_lock(&file_lock);
    fp = fopen("users.txt", "a");
    if (fp)
    {
        fputs(name, fp);
        fputc(',', fp);
        fputs(password, fp);
        fputc('\n', fp);

        fclose(fp);
    }
    pthread_mutex_unlock(&file_lock);
}

bool client_file_does_client_exist(char *name)
{
    FILE *fp;
    char line[MAX_LINE_LENGTH];
    bool exists;
    uint8_t length;

    pthread_mutex_lock(&file_lock);
    fp = fopen("users.txt", "r");
    exists = false;
    if (fp)
    {
        while (fgets(line, MAX_LINE_LENGTH, fp) && !exists)
        {
            length = strchr(line, ',') - line;
            exists = !strncmp(name, line, length);
        }
        fclose(fp);
    }
    pthread_mutex_unlock(&file_lock);

    return exists;
}

bool client_file_check_client_validity(char *name, char *password)
    {
        FILE *fp;
        char line[MAX_LINE_LENGTH];
        uint8_t length;
        bool valid;

        pthread_mutex_lock(&file_lock);
        fp = fopen("users.txt", "r");
        valid = false;
        if (fp)
        {
            while (fgets(line, MAX_LINE_LENGTH, fp) && !valid)
            {
                length = strchr(line, ',') - line;
                valid = !strncmp(name, line, length) && !strncmp(password, line + length + 1, PASSWORD_MAX_LENGTH);
            }
            fclose(fp);
        }
        pthread_mutex_unlock(&file_lock);
        return length;
    }