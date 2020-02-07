#ifndef LINKS_H
#define LINKS_H

int unlink(const char *pathname);
int unlinkat(int dirfd, const char *pathname, int flags);
#include "unlink.c"

ssize_t readlink(const char *pathname, char *buf, size_t bufsiz);
ssize_t readlinkat(int dirfd, const char *pathname, char *buf, size_t bufsiz);
#include "readlink.c"

int symlink(const char *target, const char *linkpath);
int symlinkat(const char *target, int newdirfd, const char *linkpath);
#include "symlink.c"

#endif
