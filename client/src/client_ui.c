#include "client_ui.h"

void client_ui_first_hierarchy(int sockfd);
void client_ui_second_hierarchy(int sockfd);

#define RUN_ACTION(action, sockfd)       \
    switch (action)                         \
    {                                       \
    case FIRST_HIERARCHY:                   \
        client_ui_first_hierarchy(sockfd);  \
        break;                              \
    case SECOND_HIERARCHY:                  \
        client_ui_second_hierarchy(sockfd); \
        break;                              \
    case EXIT:                              \
        close(sockfd);                      \
        exit(1);                            \
        break;                              \
    case NONE:                              \
        break;                              \
    }

/* hierarchies */

void *client_ui(void *arg)
{
    int sockfd;
    pthread_detach(pthread_self());
    sockfd = *(int *)arg;
    client.sockfd = sockfd;
    client_ui_first_hierarchy(sockfd);

    pthread_exit(NULL);
}

void client_ui_first_hierarchy(int sockfd)
{
    char option;

    printf("choose an option:\n1 - registration\n2 - login\n> ");
    scanf("%c", &option);

    if (option == '1' || option == '2')
    {
        pthread_mutex_lock(&mutex);
        is_received = false;

        option == '1' ? client_requests_register(sockfd) : client_requests_login(sockfd);
        while (!is_received)
            pthread_cond_wait(&cond, &mutex);
        pthread_mutex_unlock(&mutex);
        RUN_ACTION(action, sockfd)
    }
    else
    {
        puts("Invalid option");
        client_ui_first_hierarchy(sockfd);
    }
}

void client_ui_second_hierarchy(int sockfd)
{
    char option;
    FILE *fp;
    char file_path[FILE_PATH_MAX_LENGTH];
    char file_name[FILE_NAME_MAX_LENGTH];
    bool valid;

    pthread_mutex_lock(&mutex);

    printf("choose an option\n1 - list the files that are stored in the server\n2 - upload a file\n3 - download a file\n> ");
    scanf("%c", &option);
    is_received = false;
    valid = 1;
    switch (option)
    {
    case '1':
        client_requests_list_user_files(sockfd);
        break;
    case '2':
        printf("Enter the path of the file that you want to upload\n>");
        scanf("%s", file_path);
        client_requests_upload_file(sockfd, file_path);
        break;
    case '3':
        printf("Enter the name of the file that you want to download\n>");
        scanf("%s", file_name);
        printf("Enter the path to which you want to download the file\n>");
        scanf("%s", file_path);
        current_file = fopen(file_path, "ab");
        client_requests_download_file(sockfd, file_name);
        break;
    default:
        valid =  0;
        action = SECOND_HIERARCHY;
        puts("Invalid option");
    }

    if (valid)
    {
        while (!is_received)
            pthread_cond_wait(&cond, &mutex);
        pthread_mutex_unlock(&mutex);
    }
    RUN_ACTION(action, sockfd)
}