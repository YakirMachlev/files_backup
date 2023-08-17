#ifndef CLIENT_CONF_H
#define CLIENT_CONF_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif /* _GNU_SOURCE */

#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <inttypes.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdio.h>
#include <stdbool.h>
#include "../../protocol.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT "1234"

typedef enum
{
    FIRST_HIERARCHY = 1,
    SECOND_HIERARCHY,
    EXIT,
    NONE
} actions_e;

typedef struct
{
    int sockfd;
    char name[NAME_MAX_LENGTH];
    uint8_t name_length;
} client_t;

extern client_t client;
extern pthread_cond_t cond;
extern pthread_mutex_t mutex;
extern bool is_received;
extern actions_e action;
extern FILE *current_file;

#endif /* CLIENT_CONF_H */