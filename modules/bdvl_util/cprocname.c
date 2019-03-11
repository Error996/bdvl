#include "../libdl/libdl.h"
char *cprocname()
{
    HOOK(o_fopen, CFOPEN);
    char *pname = malloc(1024);
    FILE *cmdline;
    if((cmdline = o_fopen("/proc/self/cmdline", "r")) == NULL) return NULL;
    fread(pname, 255, 1, cmdline); fclose(cmdline);
    return pname;
}
