#ifndef SERVER_CONF_H
#define SERVER_CONF_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif /* _GNU_SOURCE */

#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <inttypes.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdbool.h>
#include <netdb.h>
#include "../../protocol.h"

#define PORT "1234"
#define MAX_CLIENTS 150
#define BACKUP_ROOT_NAME "backup_root"
#define ERROR_LENGTH 3

typedef struct
{
    int sockfd;
    char name[NAME_MAX_LENGTH];
} client_t;

#define CLIENT_DISCONNECT                                  \
    printf("Client %d disconnected\n", connected_clients); \
    close(fd);                                             \
    connected_clients--;

#define ASSERT(expression) \
    if (!(expression))     \
    {                      \
        CLIENT_DISCONNECT  \
    }

#define EXIT_PROGRAM(msg)         \
    fprintf(stderr, "%s\n", msg); \
    exit(1);

#define CREATE_DIR(dir_name)                                          \
    errno = 0;                                                        \
    if (mkdir(dir_name, S_IRWXU) == -1)                               \
    {                                                                 \
        switch (errno)                                                \
        {                                                             \
        case EACCES:                                                  \
            EXIT_PROGRAM("the parent directory does not allow write") \
        case EEXIST:                                                  \
            EXIT_PROGRAM("path name already exists")                  \
        case ENAMETOOLONG:                                            \
            EXIT_PROGRAM("pathname is too long")                      \
        default:                                                      \
            perror("mkdir");                                          \
            exit(1);                                                  \
        }                                                             \
    }

#define OPEN_DIR(dir_name)                   \
    backup_root_dir = opendir(dir_name);     \
    if (errno == ENOENT)                     \
    {                                        \
        CREATE_DIR(dir_name)                 \
        backup_root_dir = opendir(dir_name); \
    }                                        \
    if (!backup_root_dir)                    \
    {                                        \
        perror("Problem opening directory"); \
        exit(1);                             \
    }

extern DIR *backup_root_dir;
extern uint8_t connected_clients;
extern fd_set master;
extern int fdmax;
extern pthread_mutex_t mutex;

#endif /* SERVER_CONF_H */