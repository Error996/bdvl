//#include "../libdl/libdl.h"
#define CLEAN(var) memset(var, 0x00, strlen(var))
void xor(char *p)
{
    int i, key=0xAC;
    for(i=0;i<strlen(p);i++) p[i] ^= key;
}
// char *uname = gchr(STR_CONST);
/*char *gchr(char *src)
{
    char *rstr = malloc(1024);
    memset(rstr, 0x00, sizeof(rstr));
    strncpy(rstr, src, 1024);
    xor(rstr);
    return rstr;
    // CLEAN(gchr)
}*/
