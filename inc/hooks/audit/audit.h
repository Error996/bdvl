#ifndef AUDIT_H
#define AUDIT_H

int audit_log_acct_message(int audit_fd, int type, const char *pgname, const char *op, const char *name, unsigned int id, const char *host, const char *addr, const char *tty, int result);
int audit_log_user_message(int audit_fd, int type, const char *message, const char *hostname, const char *addr, const char *tty, int result);
int audit_send(int fd, int type, const void *data, unsigned int size);
#include "audit.c"

#endif