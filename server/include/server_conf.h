#ifndef SERVER_CONF_H
#define SERVER_CONF_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif /* _GNU_SOURCE */

#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
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
#define BACKUP_ROOT_PATH "backup_root"
#define DATA_MAX_LENGTH 256

typedef struct
{
    int sockfd;
    char name[NAME_MAX_LENGTH];
} client_t;

extern DIR *backup_root_dir;
extern uint8_t connected_clients;
extern fd_set master;
extern int fdmax;
extern pthread_mutex_t mutex;

#endif /* SERVER_CONF_H */