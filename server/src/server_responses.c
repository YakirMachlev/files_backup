#include "server_responses.h"

#define SEND_RESULT(fd, error, opcode, result) \
    error[0] = opcode;                             \
    error[1] = result;                             \
    send(fd, error, ERROR_LENGTH, 0);

void server_responses_register(int fd, char *buffer)
{
    char *name;
    char *password;
    uint8_t error[ERROR_LENGTH];
    uint8_t name_length;
    uint8_t password_length;

    name_length = (uint8_t)(*(buffer++));
    name = buffer;
    ASSERT(name_length < NAME_MAX_LENGTH && name_length > 0)

    buffer += name_length;
    password_length = (uint8_t)(*(buffer++));
    ASSERT(password_length < PASSWORD_MAX_LENGTH && password_length > 0)
    password = buffer;

    name[name_length] = '\0';
    password[password_length] = '\0';
    ASSERT(check_name_validity(name))

    if (!client_file_does_client_exist(name))
    {
        insert_client_to_file(name, password);
        printf("%s registered\n", name);
        SEND_RESULT(fd, error, REGISTER_RESPONSE, 1);
        printf("sent: %d,%d\n", REGISTER_RESPONSE, 1);
    }
    else
    {
        SEND_RESULT(fd, error, REGISTER_RESPONSE, 0);
        printf("sent: %d,%d\n", REGISTER_RESPONSE, 0);
    }
}

void server_responses_login(int fd, char *buffer)
{
    char *name;
    char *password;
    uint8_t error[ERROR_LENGTH];
    uint8_t name_length;
    uint8_t password_length;

    name_length = (uint8_t)(*(buffer++));
    name = buffer;
    ASSERT(name_length < NAME_MAX_LENGTH && name_length > 0)

    buffer += name_length;
    password_length = (uint8_t)(*(buffer++));
    ASSERT(password_length < PASSWORD_MAX_LENGTH && password_length > 0)
    password = buffer;

    name[name_length] = '\0';
    password[password_length] = '\0';
    ASSERT(check_name_validity(name))

    if (client_file_check_client_validity(name, password))
    {
        SEND_RESULT(fd, error, LOGIN_RESPONSE, 1);
        printf("sent: %d,%d\n", LOGIN_RESPONSE, 1);
    }
    else
    {
        SEND_RESULT(fd, error, LOGIN_RESPONSE, 0);
        printf("sent: %d,%d\n", LOGIN_RESPONSE, 0);
    }
}

void server_responses_list_user_files(int fd, char *buffer)
{
    DIR *client_folder;
    char *files_list;
    char *path[FILE_PATH_MAX_LENGTH];
    char *name;
    uint8_t name_length;
    uint8_t num_of_files;
    uint16_t total_length;

    name_length = (uint8_t)(*(buffer++));
    name = buffer;
    ASSERT(name_length < NAME_MAX_LENGTH && name_length > 0)
    name[name_length] = '\0';
    sprintf(path, "../%s", name);

    client_folder = open_folder(path);
    num_of_files = count_files_in_directory(path);
    files_list = (char *)malloc(sizeof(char) * (num_of_files * FILE_NAME_MAX_LENGTH));

    *(files_list++) = LIST_USER_FILES_RESPONSE;
    *(files_list++) = num_of_files;
    total_length = create_files_list(files_list, client_folder) + 2;
    closedir(client_folder);

    send(fd, files_list, total_length, 0);
    free(files_list);
}

void server_responses_upload_file(int fd, uint8_t *buffer)
{

}

void server_responses_download_file(int fd, uint8_t *buffer)
{

}