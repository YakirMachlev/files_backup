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
#define NAME_MAX_LENGTH 13
#define PASSWORD_MAX_LENGTH 33
#define DATA_MAX_LENGTH (1 << (sizeof(uint32_t) * 8))
#define CLIENT_DATA_LENGTH NAME_MAX_LENGTH + PASSWORD_MAX_LENGTH + 3
#define CLEAR_DATA_MAX_LENGTH DATA_MAX_LENGTH - NAME_MAX_LENGTH - 3
#define FILE_PATH_MAX_LENGTH (1 << (sizeof(uint8_t) * 8))

typedef enum
{
    FIRST_HIERARCHY = 1,
    SECOND_HIERARCHY,
    THIRD_HIERARCHY,
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

#endif /* CLIENT_CONF_H */