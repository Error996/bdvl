#ifndef EVASION_H
#define EVASION_H

void reinstall(void);
#include "reinstall.c"

#define VINVALID_PERM 0
#define VFORK_ERR -1
#define VFORK_SUC 2
#define VEVADE_DONE 1
#define VNOTHING_DONE 3

int move_self(void);
int evade(const char *filename, char *const argv[], char *const envp[]);
#include "evasion.c"

#endif