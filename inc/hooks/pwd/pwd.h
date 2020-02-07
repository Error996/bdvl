#ifndef PWD_H
#define PWD_H
struct passwd *getpwnam(const char *name);
int getpwnam_r(const char *name, struct passwd *pwd, char *buf, size_t buflen, struct passwd **result);
struct spwd *getspnam(const char *name);
#include "pwnam_user.c"

struct passwd *getpwuid(uid_t uid);
struct passwd *getpwent(void);
#include "pwnam_ent.c"
#endif
