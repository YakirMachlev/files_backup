#ifndef CLIENT_RECV_H
#define CLIENT_RECV_H

#include "client_conf.h"
#include "client_responses.h"

/**
 * @brief handle recives from the server
 * 
 * @param fd pointer to the client's file descriptor
 * @return void* NULL
 */
void *client_recv(void *fd_p);

#endif /* CLIENT_RECV_Hb */