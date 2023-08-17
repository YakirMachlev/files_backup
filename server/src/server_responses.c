#include "server_responses.h"

#define SEND_RESULT(fd, error, opcode, result) \
    error[0] = opcode;                             \
    error[1] = result;                             \
    send(fd, error, ERROR_LENGTH, 0);

void server_responses_register(int fd, uint8_t *buffer)
{
    char *name;
    char *password;
    uint8_t error[ERROR_LENGTH];
    uint8_t name_length;
    uint8_t password_length;

    name_length = *(buffer++);
    ASSERT(name_length < NAME_MAX_LENGTH && name_length > 0)

    buffer += name_length;
    password_length = *(buffer++);
    ASSERT(password_length < PASSWORD_MAX_LENGTH && password_length > 0)
    password = (char *)buffer;

    name[name_length] = '\0';
    password[password_length] = '\0';
    ASSERT(check_name_validity(name))

    if (!client_file_does_client_exist(name))
    {
        insert_client_to_file(name, password);
        printf("%s registered\n", name);
        SEND_RESULT(fd, error, REGISTER_RESPONSE, 1);
        printf("sent: %d,%d\n", REGISTER_RESPONSE, 1);
    }
    else
    {
        SEND_RESULT(fd, error, REGISTER_RESPONSE, 0);
        printf("sent: %d,%d\n", REGISTER_RESPONSE, 0);
    }
}

void server_responses_login(int fd, uint8_t *buffer)
{
    char *name;
    char *password;
    uint8_t error[ERROR_LENGTH];
    uint8_t name_length;
    uint8_t password_length;

    name_length = *(buffer++);
    ASSERT(name_length < NAME_MAX_LENGTH && name_length > 0)

    buffer += name_length;
    password_length = *(buffer++);
    ASSERT(password_length < PASSWORD_MAX_LENGTH && password_length > 0)
    password = (char *)buffer;

    name[name_length] = '\0';
    password[password_length] = '\0';
    ASSERT(check_name_validity(name))

    if (client_file_check_client_validity(name, password))
    {
        SEND_RESULT(fd, error, LOGIN_RESPONSE, 1);
        printf("sent: %d,%d\n", LOGIN_RESPONSE, 1);
    }
    else
    {
        SEND_RESULT(fd, error, LOGIN_RESPONSE, 0);
        printf("sent: %d,%d\n", LOGIN_RESPONSE, 0);
    }
}

void server_responses_list_user_files(int fd)
{
    uint8_t rooms_list[NUM_OF_ROOMS + 2];
    uint8_t error[ERROR_LENGTH];
    int offset;

    rooms_list[0] = LIST_ROOMS_RESPONSE;
    rooms_list[1] = NUM_OF_ROOMS;
    if (client->state == CONNECTED)
    {
        get_rooms_list(client, rooms_list + 2);
        send(client->sockfd, rooms_list, sizeof(rooms_list) / sizeof(uint8_t), 0);

        printf("sent: %d,%d,", rooms_list[0], rooms_list[1]);
        for (offset = 0; offset < NUM_OF_ROOMS; offset++)
        {
            printf("%d ", rooms_list[2 + offset]);
        }
        puts("");
    }
    else
    {
        SEND_RESULT(client, error, LIST_ROOMS_RESPONSE, -1);
        printf("sent: %d,%d\n", LIST_ROOMS_RESPONSE, -1);
    }
}

void server_responses_upload_file(int fd, uint8_t *buffer)
{
    uint8_t room_num;
    char connection_msg[NAME_MAX_LENGTH + 13];
    uint8_t error[ERROR_LENGTH];
    uint8_t name_length;
    uint8_t total_length;
    uint8_t connection_msg_len;
    uint8_t response;

    name_length = *(buffer++);
    ASSERT(name_length < NAME_MAX_LENGTH && name_length > 0)
    buffer += name_length;

    if (client->state == CONNECTED)
    {
        SEND_RESULT(client, error, JOIN_ROOM_RESPONSE, 0);
        printf("sent: %d,%d\n", JOIN_ROOM_RESPONSE, 0);
        printf("%s joined room %d\n", client->name, room_num + 1);

        add_client_to_room(client, room_num);
        client->state = JOINED;
        client->room_id = room_num;

        response = SEND_SERVER_MESSAGE_IN_ROOM;
        connection_msg_len = name_length + 11;
        total_length = sprintf(connection_msg, "%c%c%s connected.", response, connection_msg_len, client->name);
        client_send_massage_in_room(client, (uint8_t *)connection_msg, (int)total_length);
    }
    else
    {
        SEND_RESULT(client, error, JOIN_ROOM_RESPONSE, -1);
        printf("sent: %d,%d\n", JOIN_ROOM_RESPONSE, -1);
    }
}

void server_responses_download_file(int fd, uint8_t *buffer)
{

}