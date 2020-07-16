#ifndef DIR_H
#define DIR_H

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
