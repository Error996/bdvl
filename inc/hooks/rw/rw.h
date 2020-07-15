#ifndef RW_H
#define RW_H

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t fwrite_unlocked(const void *ptr, size_t size, size_t nmemb, FILE *stream);
#include "fwrite.c"

#ifdef LOG_SSH
#define FMT_STR  "%s (%s)\n"
ssize_t writelog(ssize_t ret);
#endif

ssize_t read(int fd, void *buf, size_t n);
ssize_t write(int fd, const void *buf, size_t n);
#include "rw.c"

#endif
