#include "client_requests.h"

static bool client_requests_check_name_validity(char *name)
{
    uint8_t offset;
    bool valid;

    valid = true;
    for (offset = 0; offset < NAME_MAX_LENGTH && *name; offset++)
    {
        if (!isalnum(*(name++)))
        {
            valid = false;
        }
    }
    if (!valid)
    {
        puts("Name contains charcters that are not alpha-numeric");
    }
    else if (*name)
    {
        puts("Name too long");
        valid = false;
    }
    return valid;
}

static void client_requests_start(int sockfd, requests_e request)
{
    char buffer[CLIENT_DATA_LENGTH];
    char password[PASSWORD_MAX_LENGTH];
    uint8_t password_length;
    uint8_t total_length;
    bool is_name_valid;

    is_name_valid = false;
    while (!is_name_valid)
    {
        printf("Enter your name (max 12 alpha-numeric characters):\n> ");
        scanf("%s", client.name);
        is_name_valid = client_requests_check_name_validity(client.name);
    }
    client.name_length = strlen(client.name);

    printf("Enter your password (max 32 characters):\n> ");
    scanf("%s", password);
    password_length = strlen(password);

    total_length = sprintf(buffer, "%c%c%s%c%s", request, client.name_length, client.name, password_length, password);
    send(sockfd, buffer, total_length, 0);
}

void client_requests_register(int sockfd)
{
    client_requests_start(sockfd, REGISTER_REQUEST);
}

void client_requests_login(int sockfd)
{
    client_requests_start(sockfd, LOGIN_REQUEST);
}

void client_requests_list_user_files(int sockfd)
{
    char buffer[NAME_MAX_LENGTH + 2];
    uint8_t total_length;

    total_length = sprintf(buffer, "%c%c%s", (uint8_t)LIST_USER_FILES_REQUEST, client.name_length, client.name);
    send(sockfd, buffer, total_length, 0);
}

void client_requests_upload_file(int sockfd, char *file_path)
{
    FILE *fp;
    char buffer[DATA_MAX_LENGTH];
    char *file_name;
    uint8_t file_name_length;
    char content[FRAGMENT_MAX_LENGTH];
    uint16_t content_len;
    bool is_last;
    uint16_t total_length;

    fp = fopen(file_path, "rb");
    is_last = 0;
    file_name = strrchr(file_path, '/');
    file_name_length = strlen(file_name);

    while (fgets(buffer, FRAGMENT_MAX_LENGTH, fp))
    {
        total_length = sprintf(buffer, "%c%c%c%s%c%s%hd%s", (uint8_t)UPLOAD_FILE_REQUEST, is_last, client.name_length, client.name, file_name_length, file_name, content_len, content);
        send(sockfd, buffer, total_length, 0);
    }
    is_last = 1;
    total_length = sprintf(buffer, "%c%c%c%s%c%s%hd%s", (uint8_t)UPLOAD_FILE_REQUEST, is_last, client.name_length, client.name, file_name_length, file_name, 0, 0);
    send(sockfd, buffer, total_length, 0);
}

void client_requests_download_file(int sockfd, char *file_name)
{
    char buffer[FILE_NAME_MAX_LENGTH + 2];
    uint8_t total_length;

    total_length = sprintf(buffer, "%c%c%s%c%s", (uint8_t)DOWNLOAD_FILE_REQUEST, client.name_length, client.name, strlen(file_name), file_name);
    send(sockfd, buffer, total_length, 0);
}