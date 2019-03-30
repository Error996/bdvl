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

char *cuname(void)
{
	char *un=malloc(128);
	HOOK(o_getpwuid,CGETPWUID);
	strncpy(un,o_getpwuid(getuid())->pw_name,strlen(o_getpwuid(getuid())->pw_name));
	return un;
}