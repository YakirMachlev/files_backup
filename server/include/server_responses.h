#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "clients_file.h"

void server_responses_register(int fd, char *buffer);
void server_responses_login(int fd, char *buffer);
void server_responses_list_user_files(int fd, char *buffer);
void server_responses_upload_file(int fd, char *buffer);
void server_responses_download_file(int fd, char *buffer);