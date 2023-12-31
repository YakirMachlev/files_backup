#include "clients.h"

void handle_client_request(int fd, char *buffer)
{
    uint8_t opcode;

    opcode = *(buffer++);
    printf("request: %d\n", opcode);
    switch (opcode)
    {
    case REGISTER_REQUEST:
        server_responses_register(fd, buffer);
        break;
    case LOGIN_REQUEST:
        server_responses_login(fd, buffer);
        break;
    case LIST_USER_FILES_REQUEST:
        server_responses_list_user_files(fd, buffer);
        break;
    case UPLOAD_FILE_REQUEST:
        server_responses_upload_file(fd, buffer);
        break;
    case DOWNLOAD_FILE_REQUEST:
        server_responses_download_file(fd, buffer);
        break;
    default:
        ASSERT(0)
    }
}

void *clients_handler()
{
    fd_set read_fds;
    int current_fd;
    int bytes_received;
    char buffer[DATA_MAX_LENGTH];
    struct timeval tv;

    FD_ZERO(&master);
    FD_ZERO(&read_fds);
    tv.tv_sec = 0;
    tv.tv_usec = 500000;

    while (true)
    {
        read_fds = master;
        if (select(fdmax + 1, &read_fds, NULL, NULL, &tv) == -1)
        {
            perror("select");
            exit(1);
        }

        for (current_fd = 0; current_fd <= fdmax; current_fd++)
        {
            if (FD_ISSET(current_fd, &read_fds))
            {
                if ((bytes_received = recv(current_fd, buffer, DATA_MAX_LENGTH, 0)) <= 0)
                {
                    if (bytes_received == 0)
                    {
                        printf("server: socket %d hung up\n", current_fd);
                    }
                    else
                    {
                        perror("recv");
                    }
                    close(current_fd);
                    FD_CLR(current_fd, &master);
                }
                else
                {
                    buffer[bytes_received] = '\0';
                    handle_client_request(current_fd, buffer);
                }
            }
        }
    }

    pthread_exit(NULL);
}