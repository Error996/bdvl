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

int xprintf(const char *string){
    char str[512];
    int ret;

    xor(_string, string);     /* let's just append a newline here... */
    (void)snprintf(str, sizeof(str) - 1, "%s\n", _string);
    ret = printf(str);
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