#include "client_responses.h"

/* responses */

void client_responses_register(char *buffer)
{
    int8_t result;

    pthread_mutex_lock(&mutex);
    result = *(buffer++);
    if (result == 0 && !*buffer)
    {
        puts("Registration succeded\n");
        action = FIRST_HIERARCHY;
    }
    else if (result == -1 && !*buffer)
    {
        puts("Registration failed\n");
        action = FIRST_HIERARCHY;
    }
    else
    {
        puts("Invalid response. Shuting down the connection");
        action = EXIT;
    }
    is_received = true;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
}

void client_responses_login(char *buffer)
{
    int8_t result;

    pthread_mutex_lock(&mutex);
    result = *buffer;
    if (result == 0 && !*buffer)
    {
        puts("Login succeded\n");
        action = SECOND_HIERARCHY;
    }
    else if (result == -1 && !*buffer)
    {
        puts("Wrong login details\n");
        action = FIRST_HIERARCHY;
    }
    else
    {
        puts("Invalid response. Shuting down the connection");
        action = EXIT;
    }
    is_received = true;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
}

void client_responses_list_files(char *buffer)
{
    int8_t result;
    uint8_t offset;
    uint8_t file_name_len;
    char file_path[FILE_PATH_MAX_LENGTH];
    char *file_name;

    pthread_mutex_lock(&mutex);
    result = *buffer;
    if (result == -1)
    {
        puts("List files failed");
        action = SECOND_HIERARCHY;
    }
    else
    {
        for (offset = 1; offset <= result; offset++)
        {
            file_name_len = *(buffer++);
            strncpy(file_path, buffer, file_name_len);
            file_name = strrchr(file_path, '/');
            /* file_name doesn't contain \0 - will cause buffer overflow */
            printf("%s\n", file_name);
            buffer += file_name_len;
        }
        action = SECOND_HIERARCHY;
    }
    puts("");
    is_received = true;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
}

void client_responses_upload_file(char *buffer)
{
    int8_t result;
    pthread_mutex_lock(&mutex);
    result = *buffer;
    if (result == 0 && !*buffer)
    {
        puts("File upload succeded\n");
        action = SECOND_HIERARCHY;
    }
    else if (result == -1 && !*buffer)
    {
        puts("File upload failed\n");
        action = SECOND_HIERARCHY;
    }
    else
    {
        puts("Invalid response. Shuting down the connection");
        action = EXIT;
    }
    is_received = true;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
}

void client_responses_download_file(char *buffer)
{
    char *file_path;
    char *content;
    uint8_t file_path_length;
    uint32_t content_length;

    file_path_length = *(buffer++);
    file_path = buffer;

    buffer += file_path_length;
    content_length = *(buffer++) << 24;
    content_length |= *(buffer++) << 16;
    content_length |= *(buffer++) << 8;
    content_length |= *(buffer++);
    content = buffer;

    file_path[file_path_length] = '\0';
    content[content_length] = '\0';

    /* open the file path and write the content in binary to the file */
}

void client_responses_server_full(char *buffer)
{
    int8_t msg_length;

    pthread_mutex_lock(&mutex);
    msg_length = *(buffer++);
    /* file_name doesn't contain \0 - will cause buffer overflow */
    printf("%s\n", buffer);
    action = EXIT;
    is_received = true;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
}