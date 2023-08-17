#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <ctype.h>
#include <string.h>
#include <pthread.h>
#include "server_conf.h"

bool check_name_validity(char *name);
void insert_client_to_file(char *name, char *password);
bool client_file_does_client_exist(char *name);
bool client_file_check_client_validity(char *name, char *password);