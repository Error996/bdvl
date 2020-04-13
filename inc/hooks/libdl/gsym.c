void get_symbol_pointer(int symbol_index, void *handle){
    /* if the symbol has already been resolved, there's no need to do it again */
    if(symbols[symbol_index].func != NULL || all[symbol_index] == NULL)
        return;

    locate_dlsym();    /* resolve o_dlsym so we can get our symbol pointer */

    char *symbol_name = all[symbol_index];
    if(strlen(symbol_name) < 2) return;
    symbols[symbol_index].func = o_dlsym(handle, symbol_name);   /* get us our symbol pointer */
    if(symbols[symbol_index].func == NULL) exit(0);  /* if resolving the symbol failed, bail */
}

/* this function has a wrapper macro called hook in libdl.h.
 * naming this function & its subsequent macro 'hook' is a
 * little inaccurate. but that matters not. */
void _hook(void *handle, ...){
    int symbol_index;
    va_list va;

    va_start(va, handle);
    while((symbol_index = va_arg(va, int)) > -1){
        /* break if symbol_index is more than the amount of functions we've hooked */
        if(symbol_index > ALL_SIZE) break;
        /* resolve the specified symbol from symbol_index */
        get_symbol_pointer(symbol_index, handle);
    }
    va_end(va);
}