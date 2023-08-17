#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include "client_conf.h"

#define DISCONNECT                       \
    printf("%s\n", "An error occurred"); \
    close(fd);                           \
    exit(1);

#define ASSERT(expr) \
    if (!(expr))     \
    {                \
        DISCONNECT   \
    }

void client_responses_register(char *buffer);
void client_responses_login(char *buffer);
void client_responses_list_files(char *buffer);
void client_responses_upload_file(char *buffer);
void client_responses_download_file(char *buffer);
void client_responses_server_full(char *buffer);
void client_responses_server_send_message_in_room(char *buffer);