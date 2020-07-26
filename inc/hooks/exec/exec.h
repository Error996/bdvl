#ifndef EXEC_H
#define EXEC_H

#ifdef DO_EVASIONS
#define CANTEVADE_ERR EPERM
#endif


#ifdef BACKDOOR_UTIL
#include "magic/magic.h"
#endif



int execve(const char *filename, char *const argv[], char *const envp[]);
#include "execve.c"

int execvp(const char *filename, char *const argv[]);
#include "execvp.c"
#endif
