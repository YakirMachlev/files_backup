#include "client_ui.h"

void client_ui_first_hierarchy(int sockfd);
void client_ui_second_hierarchy(int sockfd);

#define RUN_ACTION(hierarchy, sockfd)       \
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
    char file_path[FILE_PATH_MAX_LENGTH];

    printf("choose an option\n1 - list the files that are stored in the server\n2 - upload a file\n3 - download a file\n> ");
    scanf("%c", &option);

    pthread_mutex_lock(&mutex);
    is_received = false;
    if (option == '1')
    {
        client_requests_list_user_files(sockfd);
        while (!is_received)
            pthread_cond_wait(&cond, &mutex);
        pthread_mutex_unlock(&mutex);
        RUN_ACTION(action, sockfd)
    }
    else if (option == '2' || option == '3')
    {
        printf("Enter the file path\n>");
        scanf("%s", file_path);
        option == '2' ? client_requests_upload_file(sockfd, file_path) : client_requests_download_file(sockfd, file_path);

        while (!is_received)
            pthread_cond_wait(&cond, &mutex);
        pthread_mutex_unlock(&mutex);
        RUN_ACTION(action, sockfd)
    }
    else
    {
        puts("Invalid option");
        client_ui_second_hierarchy(sockfd);
    }
}