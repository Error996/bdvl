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
char *procinfo(pid_t pid, int mode){
    char *procinfo;
    int fd, c;

    hook(CREAD);

    fd = open_cmdline(pid);
    if(fd < 0){
        size_t fallbacklen = strlen(FALLBACK_PROCNAME);
        procinfo = malloc(fallbacklen+1);
        if(!procinfo) return FALLBACK_PROCNAME;
        memset(procinfo, 0, fallbacklen+1);
        strncpy(procinfo, FALLBACK_PROCNAME, fallbacklen);
        goto end_processinfo;
    }

    switch(mode){
        case MODE_NAME:
            procinfo = malloc(NAME_MAXLEN+1);
            if(!procinfo){
                close(fd);
                return NULL;
            }
            memset(procinfo, 0, NAME_MAXLEN+1);
            c = (long)call(CREAD, fd, procinfo, NAME_MAXLEN);
            break;
        case MODE_CMDLINE:
            procinfo = malloc(CMDLINE_MAXLEN+1);
            if(!procinfo){
                close(fd);
                return NULL;
            }
            memset(procinfo, 0, CMDLINE_MAXLEN+1);
            c = (long)call(CREAD, fd, procinfo, CMDLINE_MAXLEN);
            for(int i = 0; i < c; i++)
                if(procinfo[i] == 0x00)
                    procinfo[i] = 0x20;
            procinfo[c-1]='\0';
            break;
    }

    close(fd);
end_processinfo:
    return procinfo;
}
