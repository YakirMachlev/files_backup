#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <pthread.h>
#include "client_conf.h"

void client_requests_register(int sockfd);
void client_requests_login(int sockfd);
void client_requests_list_user_files(int sockfd);
void client_requests_upload_file(int sockfd, char *file_path);
void client_requests_download_file(int sockfd, char *file_name);
