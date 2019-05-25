//#include "../libdl/libdl.h"
#define CLEAN(var) memset(var, 0x00, strlen(var))
void xor(char *p)
{
    int i;
    for(i=0;i<strlen(p);i++) p[i] ^= XKEY;
}
