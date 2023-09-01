#include "client_responses.h"

/* responses */

void client_responses_register(char *buffer)
{
    uint8_t result;

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
}

void client_responses_login(char *buffer)
{
    uint8_t result;

    result = *(buffer++);
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
}

void client_responses_list_files(char *buffer)
{
    uint8_t result;
    uint8_t offset;
    uint8_t file_name_len;
    uint8_t next_file_name_len;

    result = *(buffer++);
    action = SECOND_HIERARCHY;
    if (result == 0 && !*buffer)
    {
        puts("List files failed");
    }
    else
    {
        /* result is the amount of files */
        puts("Your files are:");
        file_name_len = *(buffer++);
        for (offset = 0; offset < result; offset++)
        {
            next_file_name_len = *(buffer + file_name_len);
            *(buffer + file_name_len) = '\0';
            printf("%s\n", buffer);

            buffer += file_name_len + 1;
            file_name_len = next_file_name_len;
        }
        puts("");
    }
}

void client_responses_upload_file(char *buffer)
{
    uint8_t result;
    result = *(buffer++);
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
}

void client_responses_download_file(char *buffer)
{
    bool is_last;
    uint16_t content_length;

    action = NONE;
    is_last = *(buffer++);
    content_length = *(buffer++) << 8;
    content_length |= *(buffer++);

    if (content_length)
    {
        /* fwrite(buffer, content_length, 1, current_file); */
        fputs(buffer, current_file);
        if (is_last)
        {
            fclose(current_file);
            action = SECOND_HIERARCHY;
        }
        puts("downloaded the file successfully\n");
    }
    else
    {
        puts("Failed to download the given file\n");
    }
}

void client_responses_server_full(char *buffer)
{
    int8_t msg_length;

    msg_length = *(buffer++);
    buffer[msg_length] = '\0';
    printf("%s\n", buffer);
    action = EXIT;
}