//#define HOOK(o_sym, csym) if(!o_sym) o_sym = get_sym(RTLD_NEXT, csym)
//#include "../bdvl_util/util.h"
void *get_sym(void *handle, int csym)
{
    void *fptr;
    char *symbol = strdup(all_calls[csym]); xor(symbol);
    if(!o_dlsym) locate_dlsym();
    fptr = o_dlsym(handle, symbol);
    CLEAN(symbol);
    if(!fptr) exit(1);
    return fptr;
}

