#include "server_responses.h"

#define SEND_RESULT(fd, response, opcode, valid)     \
    response[0] = opcode;                            \
    response[1] = valid;                             \
    response[2] = '\0';                              \
    send(fd, response, VALIDITY_RESPONSE_LENGTH, 0); \
    printf("sent: %d,%d\n", opcode, valid);

void server_responses_register(int fd, char *buffer)
{
    char *name;
    char *password;
    uint8_t response[VALIDITY_RESPONSE_LENGTH];
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
        SEND_RESULT(fd, response, REGISTER_RESPONSE, 1);
    }
    else
    {
        SEND_RESULT(fd, response, REGISTER_RESPONSE, 0);
    }
}

void server_responses_login(int fd, char *buffer)
{
    char *name;
    char *password;
    uint8_t response[VALIDITY_RESPONSE_LENGTH];
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
        SEND_RESULT(fd, response, LOGIN_RESPONSE, 1);
    }
    else
    {
        SEND_RESULT(fd, response, LOGIN_RESPONSE, 0);
    }
}

void server_responses_list_user_files(int fd, char *buffer)
{
    DIR *client_folder;
    char *files_list;
    char *name;
    uint8_t response[VALIDITY_RESPONSE_LENGTH];
    uint8_t name_length;
    uint8_t num_of_files;
    uint16_t total_length;

    name_length = (uint8_t)(*(buffer++));
    name = buffer;
    ASSERT(name_length < NAME_MAX_LENGTH && name_length > 0)
    name[name_length] = '\0';

    client_folder = open_folder(name);
    if (client_folder)
    {
        num_of_files = count_files_in_directory(client_folder);
        files_list = (char *)malloc(sizeof(char) * (num_of_files * FILE_NAME_MAX_LENGTH));

        *files_list = (uint8_t)LIST_USER_FILES_RESPONSE;
        *(files_list + 1) = num_of_files;
        rewinddir(client_folder);
        total_length = create_files_list(files_list + 2, client_folder) + 2;
        closedir(client_folder);
        send(fd, files_list, total_length, 0);
        free(files_list);
    }
    else
    {
        SEND_RESULT(fd, response, LIST_USER_FILES_RESPONSE, 0);
    }
}

void server_responses_upload_file(int fd, char *buffer)
{
    FILE *fp;
    uint8_t is_last;
    uint8_t name_length;
    char *name;
    uint8_t file_name_length;
    char *file_name;
    uint16_t content_len;
    char *content;
    char response[VALIDITY_RESPONSE_LENGTH];
    char path[FILE_PATH_MAX_LENGTH];

    is_last = (uint8_t)(*(buffer++));
    name_length = (uint8_t)(*(buffer++));
    ASSERT(name_length < NAME_MAX_LENGTH && name_length > 0)
    name = buffer;

    buffer += name_length;
    file_name_length = (uint8_t)(*(buffer++));
    ASSERT(file_name_length < FILE_NAME_MAX_LENGTH && file_name_length > 0)
    file_name = buffer;

    buffer += file_name_length;
    content_len = ((u_int8_t)(*(buffer++))) << 8;
    content_len |= (u_int8_t)(*(buffer++));
    content = buffer;

    name[name_length] = '\0';
    file_name[file_name_length] = '\0';
    content[content_len] = '\0';
    printf("received: %d,%d,%s,%d,%s,%hd,%s\n", is_last, name_length, name, file_name_length, file_name, content_len, content);
    sprintf(path, "%s/%s", name, file_name);

    CREATE_DIR(name)
    fp = fopen(path, "ab");
    if (fp)
    {
        fputs(content, fp);

        if (is_last)
        {
            fclose(fp);
        }
        SEND_RESULT(fd, response, (uint8_t)UPLOAD_FILE_RESPONSE, 1);
    }
    else
    {
        SEND_RESULT(fd, response, (uint8_t)UPLOAD_FILE_RESPONSE, 0);
    }
}

uint16_t file_read(char *buffer, uint16_t size, FILE *stream)
{
    uint16_t offset;
    char c;
    for (offset = 0; offset < size; offset++)
    {
        if ((c = getc(stream)) == EOF)
        {
            break;
        }
        buffer[offset] = c;
    }
    return offset + 1;
}

void server_responses_download_file(int fd, char *buffer)
{
    FILE *fp;
    char send_buffer[FRAGMENT_MAX_LENGTH + 5];
    char *name;
    uint8_t name_length;
    char *file_name;
    uint8_t file_name_length;
    char content[FRAGMENT_MAX_LENGTH];
    uint16_t content_len;
    uint8_t is_last;
    uint16_t total_length;
    char path[FILE_PATH_MAX_LENGTH];

    name_length = (uint8_t)(*(buffer++));
    ASSERT(name_length < NAME_MAX_LENGTH && name_length > 0)
    name = buffer;

    buffer += name_length;
    file_name_length = (uint8_t)(*(buffer++));
    ASSERT(file_name_length < FILE_NAME_MAX_LENGTH && file_name_length > 0)
    file_name = buffer;

    name[name_length] = '\0';
    file_name[file_name_length] = '\0';

    sprintf(path, "%s/%s", name, file_name);
    fp = fopen(path, "rb");

    if (fp)
    {
        is_last = 0;
        while ((content_len = file_read(content, FRAGMENT_MAX_LENGTH, fp)) == FRAGMENT_MAX_LENGTH)
        {
            total_length = sprintf(send_buffer, "%c%c%c%c%s", (uint8_t)DOWNLOAD_FILE_RESPONSE, is_last, (content_len >> 8) & 0xFF, content_len & 0xFF, content);
            send(fd, send_buffer, total_length, 0);
        }
        is_last = 1;
        total_length = sprintf(send_buffer, "%c%c%c%c%s", (uint8_t)DOWNLOAD_FILE_RESPONSE, is_last, (content_len >> 8) & 0xFF, content_len & 0xFF, content);
        send(fd, send_buffer, total_length, 0);
    }
    else
    {
        total_length = sprintf(send_buffer, "%c%c%c%c", (uint8_t)DOWNLOAD_FILE_RESPONSE, 0, 0, 0);
        send(fd, send_buffer, total_length, 0);
    }
}