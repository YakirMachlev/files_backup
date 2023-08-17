#include "client_recv.h"

static void handle_recv_from_server(char *buffer, int length)
{
    responses_e response;

    response = (uint8_t) * (buffer++);
    switch (response)
    {
    case REGISTER_RESPONSE:
        client_responses_register(buffer);
        break;
    case LOGIN_RESPONSE:
        client_responses_login(buffer);
        break;
    case LIST_USER_FILES_RESPONSE:
        client_responses_list_files(buffer);
        break;
    case UPLOAD_FILE_RESPONSE:
        client_responses_upload_file(buffer);
        break;
    case DOWNLOAD_FILE_RESPONSE:
        client_responses_download_file(buffer);
        break;
    case SERVER_FULL_RESPONSE:
        client_responses_server_full(buffer);
        break;
    default:
        puts("Invalid response. Shuting down the connection");
        close(client.sockfd);
        exit(1);
        break;
    }
}

void *client_recv(void *arg)
{
    char buffer[DATA_MAX_LENGTH];
    int bytes_received;
    int fd;

    fd = *(int *)arg;
    while (true)
    {
        bytes_received = recv(fd, buffer, DATA_MAX_LENGTH, 0);
        if (bytes_received <= 0)
        {
            if (bytes_received == 0)
            {
                printf("server: socket %d hung up\n", fd);
            }
            else
            {
                perror("recv");
            }
            DISCONNECT
        }
        buffer[bytes_received] = '\0';
        handle_recv_from_server(buffer, bytes_received);
    }

    pthread_exit(NULL);
}