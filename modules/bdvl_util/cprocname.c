#include "../libdl/libdl.h"
char *cprocname()
{
    HOOK(o_fopen, CFOPEN);
    char *pname = malloc(1024);
    FILE *cmdline;
    if((cmdline = o_fopen("/proc/self/cmdline", "r")) == NULL)
    {
        snprintf(pname,5,"bash");
    }else{
        fread(pname, 255, 1, cmdline);
        fclose(cmdline);
    }
    
    return pname;
}

char *gcmdline()
{
    HOOK(o_open,COPEN);
    HOOK(o_read,CREAD);
    char *cmdline=malloc(2048),path[256];
    int i, c, fd;
    snprintf(path, sizeof(path), "/proc/%d/cmdline", getpid());
    if((fd=o_open(path,0)) < 0)
    {
        snprintf(cmdline,5,"bash");
    }else{
        c=o_read(fd,cmdline,sizeof(cmdline));
        for(i=0;i<c;i++) if(cmdline[i]==0x00) cmdline[i]=0x20;
    }
    if(fd) close(fd);
    return cmdline;
}

char *cuname(void)
{
	char *un=malloc(128);
	HOOK(o_getpwuid,CGETPWUID);
	strncpy(un,o_getpwuid(getuid())->pw_name,strlen(o_getpwuid(getuid())->pw_name));
	return un;
}