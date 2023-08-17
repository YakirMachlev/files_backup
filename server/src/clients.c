#include "clients.h"

void handle_client_request(int fd, char *buffer)
{
    uint8_t opcode;
    int bytes_received;

    opcode = *(buffer++);
    printf("request: %d\n", opcode);
    switch (opcode)
    {
    case REGISTER_REQUEST:
        client_register(fd, buffer);
        break;
    case LOGIN_REQUEST:
        client_login(fd, buffer);
        break;
    case LIST_USER_FILES_REQUEST:
        client_list_rooms(fd, buffer);
        break;
    case UPLOAD_FILE_REQUEST:
        client_join_room(fd, buffer);
        break;
    case DOWNLOAD_FILE_REQUEST:
        client_send_massage_in_room(fd, buffer);
        break;
    default:
        ASSERT(0)
    }
}

void *clients_handler(void *arg)
{
    int listener;
    fd_set read_fds;
    int current_fd;
    int bytes_received;
    char buffer[DATA_MAX_LENGTH];
    struct timeval tv;

    listener = *(int *)arg;
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