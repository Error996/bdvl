#ifndef STAT_H
#define STAT_H
int stat(const char *pathname, struct stat *buf);
int stat64(const char *pathname, struct stat64 *buf);
int __xstat(int version, const char *pathname, struct stat *buf);
int __xstat64(int version, const char *pathname, struct stat64 *buf);
#include "stat.c"

int lstat(const char *pathname, struct stat *buf);
int lstat64(const char *pathname, struct stat64 *buf);
int __lxstat(int version, const char *pathname, struct stat *buf);
int __lxstat64(int version, const char *pathname, struct stat64 *buf);
#include "lstat.c"

int fstat(int fd, struct stat *buf);
int fstat64(int fd, struct stat64 *buf);
int fstatat(int dirfd, const char *pathname, struct stat *buf, int flags);
int fstatat64(int dirfd, const char *pathname, struct stat64 *buf, int flags);
int __fxstat(int version, int fd, struct stat *buf);
int __fxstat64(int version, int fd, struct stat64 *buf);
#include "fstat.c"
#endif
