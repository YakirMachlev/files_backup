#pragma once

#define __USE_MISC
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <ctype.h>
#include <string.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "server_conf.h"

DIR *open_folder(char *path);
uint16_t create_files_list(char *files_list, DIR *folder);
uint8_t count_files_in_directory(DIR *folder);
bool check_name_validity(char *name);
void insert_client_to_file(char *name, char *password);
bool client_file_does_client_exist(char *name);
bool client_file_check_client_validity(char *name, char *password);