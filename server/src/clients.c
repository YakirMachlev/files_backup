#include "clients.h"

void* clients_handler(void *arg)
{
    int listener;
    fd_set read_fds;
    int current_fd_offset;
    int offset;
    int bytes_received;
    char buffer[DATA_MAX_LENGTH];

    listener = *(int *)arg;
    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    while (true)
    {
        read_fds = master;
        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1)
        {
            perror("select");
            exit(1);
        }

        for (current_fd_offset = 0; current_fd_offset <= fdmax; current_fd_offset++)
        {
            if (FD_ISSET(current_fd_offset, &read_fds))
            {
                if ((bytes_received = recv(current_fd_offset, buffer, DATA_MAX_LENGTH, 0)) <= 0)
                {
                    if (bytes_received == 0)
                    {
                        printf("server: socket %d hung up\n", current_fd_offset);
                    }
                    else
                    {
                        perror("recv");
                    }
                    close(current_fd_offset); 
                    FD_CLR(current_fd_offset, &master); 
                }
                else
                {
                    /* sending the received data to all of the others fds 
                    for (offset = 0; offset <= fdmax; offset++)
                    {
                        if (FD_ISSET(offset, &master))
                        {
                            if (offset != current_fd_offset)
                            {
                                if (send(offset, buffer, bytes_received, 0) == -1)
                                {
                                    perror("send");
                                }
                            }
                        }
                    }*/
                }
            }
        }
    }

    pthread_exit(NULL);
}