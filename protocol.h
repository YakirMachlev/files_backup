#pragma once

#define NAME_MAX_LENGTH 12
#define PASSWORD_MAX_LENGTH 32
#define FRAGMENT_MAX_LENGTH 1024
#define FILE_NAME_MAX_LENGTH 64
#define FILE_PATH_MAX_LENGTH 256
#define DATA_MAX_LENGTH (FRAGMENT_MAX_LENGTH + NAME_MAX_LENGTH + FILE_NAME_MAX_LENGTH + 7)
#define CLIENT_DATA_LENGTH (NAME_MAX_LENGTH + PASSWORD_MAX_LENGTH + 4)
#define CLEAR_DATA_MAX_LENGTH (DATA_MAX_LENGTH - NAME_MAX_LENGTH - 3)


typedef enum
{
    REGISTER_REQUEST = 100,
    LOGIN_REQUEST,
    LIST_USER_FILES_REQUEST,
    UPLOAD_FILE_REQUEST,
    DOWNLOAD_FILE_REQUEST
} requests_e;

typedef enum
{
    REGISTER_RESPONSE = 200,
    LOGIN_RESPONSE,
    LIST_USER_FILES_RESPONSE,
    UPLOAD_FILE_RESPONSE,
    DOWNLOAD_FILE_RESPONSE,
    SERVER_FULL_RESPONSE
} responses_e;