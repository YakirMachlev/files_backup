#include "server_conf.h"
#include "clients.h"

DIR *backup_root_dir = NULL;
uint8_t connected_clients = 0;
fd_set master;
int fdmax;
pthread_mutex_t mutex;

#define SERVER_FULL_MSG "server currently full"

#define EXIT_PROGRAM(msg)         \
    fprintf(stderr, "%s\n", msg); \
    exit(1);

#define CREATE_BACKUP_ROOT_DIR                                        \
    errno = 0;                                                        \
    if (mkdir(BACKUP_ROOT_NAME, S_IRWXU) == -1)                       \
    {                                                                 \
        switch (errno)                                                \
        {                                                             \
        case EACCES:                                                  \
            EXIT_PROGRAM("the parent directory does not allow write") \
        case EEXIST:                                                  \
            EXIT_PROGRAM("the directory already exists")              \
        case ENAMETOOLONG:                                            \
            EXIT_PROGRAM("pathname is too long")                      \
        default:                                                      \
            perror("mkdir");                                          \
        }                                                             \
    }

#define OPEN_BACKUP_ROOT_DIR                         \
    backup_root_dir = opendir(BACKUP_ROOT_NAME);     \
    if (errno == ENOENT)                             \
    {                                                \
        CREATE_BACKUP_ROOT_DIR                       \
        backup_root_dir = opendir(BACKUP_ROOT_NAME); \
    }                                                \
    if (!backup_root_dir)                            \
    {                                                \
        perror("Problem opening directory");         \
        exit(1);                                     \
    }

/**
 * @brief cleans up resources before exiting the program
 *
 */
static void backup_server_cleanup()
{
    printf("\n%s\n", "Shuting down...");
    closedir(backup_root_dir);
    exit(0);
}

/**
 * @brief finds a valid socket for the server according to the
 * information specified in the given server info
 *
 * @param servinfo the server info
 * @return int the server socket file descriptor
 */
int server_find_valid_socket(struct addrinfo *servinfo)
{
    int listener;
    struct addrinfo *p;
    int yes;

    yes = 1;
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("server: socket");
            continue;
        }
        if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
            perror("setsockopt");
        }
        if (bind(listener, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(listener);
            perror("server: bind");
            continue;
        }
        break;
    }
    freeaddrinfo(servinfo);
    if (p == NULL)
    {
        EXIT_PROGRAM("server: failed to bind");
    }

    return listener;
}

/**
 * @brief gets the server's listener socket file descriptor
 *
 * @return int listener socket file descriptor on success, else NULL
 */
static int backup_server_get_listener_socket()
{
    int listener;
    struct addrinfo hints;
    struct addrinfo *server_info;
    int8_t err;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((err = getaddrinfo(NULL, PORT, &hints, &server_info)))
    {
        EXIT_PROGRAM(gai_strerror(err))
    }
    listener = server_find_valid_socket(server_info);
    if (listen(listener, MAX_CLIENTS + 1) == -1)
    {
        EXIT_PROGRAM("Error getting listening socket")
    }

    return listener;
}

/**
 * @brief handle accepts to the server
 *
 * @param listener_p a pointer to the server's listener socket
 * @return void*
 */
static void *backup_server_acceptor(void *arg)
{
    int listener;
    int client_sockfd;
    char server_full_buffer[24];

    puts("Waiting for connections...");
    sprintf(server_full_buffer, "%c%c%21s", (uint8_t)SERVER_FULL_RESPONSE, 21, SERVER_FULL_MSG);
    listener = *(int *)arg;
    while (true)
    {
        client_sockfd = accept(listener, NULL, NULL);

        if (client_sockfd == -1)
        {
            perror("accept");
        }
        else if (connected_clients > MAX_CLIENTS)
        {
            send(client_sockfd, server_full_buffer, 24, 0);
            close(client_sockfd);
            connected_clients--;
        }
        else
        {
            pthread_mutex_lock(&mutex);
            connected_clients++;
            FD_SET(client_sockfd, &master);
            if (client_sockfd > fdmax)
            {
                fdmax = client_sockfd;
            }
            pthread_mutex_unlock(&mutex);
        }
    }

    pthread_exit(NULL);
}

/**
 * @brief runs the backup server
 *
 */
static void backup_server_run()
{
    pthread_t acceptor_thread;
    pthread_t clients_handler_thread;
    int listener;

    OPEN_BACKUP_ROOT_DIR
    chdir(BACKUP_ROOT_NAME);

    signal(SIGINT, backup_server_cleanup);
    listener = backup_server_get_listener_socket();

    pthread_create(&acceptor_thread, NULL, backup_server_acceptor, &listener);
    pthread_create(&clients_handler_thread, NULL, clients_handler, NULL);
    pthread_join(acceptor_thread, NULL);
    pthread_join(clients_handler_thread, NULL);
}

int main(int argc, char *argv[])
{
    char *last_slash;

    last_slash = strrchr(*argv, '/');
    if (last_slash)
    {
        *last_slash = '\0';
        chdir(*argv);
    }
    backup_server_run();
    return 0;
}