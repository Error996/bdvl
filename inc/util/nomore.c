int rknomore(void){
    DIR *dp;
    struct dirent *dir;
    int status = 1;

    hook(COPENDIR, CREADDIR);

    dp = call(COPENDIR, INSTALL_DIR);
    if(dp == NULL) // if we cant open installdir, rk-is-no-more
        return 1;

    while((dir = call(CREADDIR, dp)) != NULL){
        if(!strncmp(".", dir->d_name, 1))
            continue;

        // if we can detect the kits shared object, rk-is-more
        if(!strncmp(BDVLSO, dir->d_name, strlen(BDVLSO))){
            status = 0;
            break;
        }
    }
    closedir(dp);

    return status;
}