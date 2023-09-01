#include "client_conf.h"
#include "client_recv.h"
#include "client_ui.h"

#define EXIT_PROGRAM(msg)         \
    fprintf(stderr, "%s\n", msg); \
    exit(1);

client_t client;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
bool is_received = false;
actions_e action;
FILE *current;

static int client_find_server_valid_socket(struct addrinfo *servinfo)
{
    int sockfd;
    struct addrinfo *p;

    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("client: socket");
            continue;
        }
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("client: connect");
            continue;
        }
        break;
    }
    freeaddrinfo(servinfo);
    if (p == NULL)
    {
        EXIT_PROGRAM("client: failed to connect")
    }

    return sockfd;
}

/**
 * @brief gets the client's socket file descriptor
 *
 * @return int client socket file descriptor on success, else NULL
 */
static int backup_client_get_socket()
{
    int fd;
    struct addrinfo hints;
    struct addrinfo *server_info;
    int8_t err;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((err = getaddrinfo(SERVER_IP, SERVER_PORT, &hints, &server_info)) != 0)
    {
        EXIT_PROGRAM(gai_strerror(err))
    }
    fd = client_find_server_valid_socket(server_info);

    return fd;
}

/**
 * @brief cleans up resources before exiting the program
 *
 */
static void backup_client_cleanup()
{
    printf("\n%s\n", "Disconnecting...");
    exit(1);
}

/**
 * @brief runs the backup client
 *
 */
static void client_run()
{
    pthread_t recv_thread;
    pthread_t ui_thread;
    int fd;

    signal(SIGINT, backup_client_cleanup);
    fd = backup_client_get_socket();

    pthread_create(&recv_thread, NULL, client_recv, &fd);
    pthread_create(&ui_thread, NULL, client_ui, &fd);
    pthread_join(recv_thread, NULL);
    pthread_join(ui_thread, NULL);
}

int main()
{
    client_run();
    return 0;
}