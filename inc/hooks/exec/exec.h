#ifndef EXEC_H
#define EXEC_H

#ifdef LOG_USER_EXEC
void peepargv(char *const argv[]);
#include "peep.c"
#endif


int execve(const char *filename, char *const argv[], char *const envp[]);
#include "execve.c"

int execvp(const char *filename, char *const argv[]);
#include "execvp.c"
#endif
