#ifndef CLIENTS_H
#define CLIENTS_H

#include "server_conf.h"

/**
 * @brief manage the clients connection to the server
 * 
 * @param listener_p pointer to the server listener file descriptor
 * @return void* NULL
 */
void* clients_handler(void *listener_p);

#endif /* CLIENTS_H */