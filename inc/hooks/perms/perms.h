#ifndef PERMS_H
#define PERMS_H
int chown(const char *pathname, uid_t owner, gid_t group);
#include "chown.c"

int chmod(const char *pathname, mode_t mode);
int fchmod(int fd, mode_t mode);
int fchmodat(int dirfd, const char *pathname, mode_t mode, int flags);
#include "chmod.c"
#endif
