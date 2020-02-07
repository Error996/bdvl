#ifndef GID_H
#define GID_H
int setgid(gid_t gid);
#include "setgid.c"

int setegid(gid_t egid);
#include "setegid.c"

int setregid(gid_t rgid, gid_t egid);
#include "setregid.c"

int setresgid(gid_t rgid, gid_t egid, gid_t sgid);
#include "setresgid.c"
#endif
