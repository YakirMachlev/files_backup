#include "client_responses.h"

/* responses */

void client_responses_register(char *buffer)
{
    uint8_t result;

    pthread_mutex_lock(&mutex);
    result = *(buffer++);
    if (result == 1 && !*buffer)
    {
        puts("Registration succeded\n");
        action = FIRST_HIERARCHY;
    }
    else if (result == 0 && !*buffer)
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
    uint8_t result;

    pthread_mutex_lock(&mutex);
    result = *buffer;
    if (result == 1 && !*buffer)
    {
        puts("Login succeded\n");
        action = SECOND_HIERARCHY;
    }
    else if (result == 0 && !*buffer)
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
    uint8_t result;
    uint8_t offset;
    uint8_t file_name_len;
    char file_name[FILE_NAME_MAX_LENGTH];

    pthread_mutex_lock(&mutex);
    result = *buffer;
    action = SECOND_HIERARCHY;
    if (result == 0)
    {
        puts("List files failed");
    }
    else
    {
        /* result is the amount of files */
        for (offset = 0; offset < result; offset++)
        {
            file_name_len = *(buffer++);
            strncpy(file_name, buffer, file_name_len);
            file_name[file_name_len] = '\0';
            printf("%s\n", file_name);

            buffer += file_name_len;
        }
        puts("");
    }
    is_received = true;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
}

void client_responses_upload_file(char *buffer)
{
    uint8_t result;
    pthread_mutex_lock(&mutex);
    result = *buffer;
    action = SECOND_HIERARCHY;
    if (result == 1 && !*buffer)
    {
        puts("File upload succeded\n");
    }
    else if (result == 0 && !*buffer)
    {
        puts("File upload failed\n");
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
    bool is_last;
    uint16_t content_length;

    pthread_mutex_lock(&mutex);
    is_last = *(buffer++);
    content_length = *(buffer++) << 8;
    content_length |= *(buffer++);

    action = SECOND_HIERARCHY;
    fwrite(buffer, content_length, 1, current_file);
    if (is_last)
    {
        fclose(current_file);
        is_received = true;
        pthread_cond_signal(&cond);
    }
    if (content_length == 0)
    {
        puts("Failed to download the given file");
        action = EXIT;
    }
    pthread_mutex_unlock(&mutex);
}

void client_responses_server_full(char *buffer)
{
    int8_t msg_length;

    pthread_mutex_lock(&mutex);
    msg_length = *(buffer++);
    buffer[msg_length] = '\0';
    printf("%s\n", buffer);
    action = EXIT;
    is_received = true;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
}