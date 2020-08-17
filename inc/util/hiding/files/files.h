#ifndef FILES_H
#define FILES_H

#define mxstat(pathname, s)    (long)call(C__XSTAT, _STAT_VER, pathname, s)    /* regular path stat */
#define mxstat64(pathname, s)  (long)call(C__XSTAT64, _STAT_VER, pathname, s)  /* large files       */
#define mfxstat(fd, s)         (long)call(C__FXSTAT, _STAT_VER, fd, s)         /* fildes stat       */
#define mfxstat64(fd, s)       (long)call(C__FXSTAT64, _STAT_VER, fd, s)       /* large files...    */
#define mlxstat(pathname, s)   (long)call(C__LXSTAT, _STAT_VER, pathname, s)   /* link stat         */
#define mlxstat64(pathname, s) (long)call(C__LXSTAT64, _STAT_VER, pathname, s) /* ...               */
gid_t get_path_gid(const char *pathname);
gid_t get_path_gid64(const char *pathname);
gid_t lget_path_gid(const char *pathname);
gid_t lget_path_gid64(const char *pathname);
gid_t get_fd_gid(int fd);
gid_t get_fd_gid64(int fd);
#include "get_path_gid.c"

#include "hidden.c"



#endif