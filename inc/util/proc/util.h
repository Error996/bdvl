int sshdproc(void){
    int sshd=0;
    char *myname = procname();
    if(myname != NULL){
        if(!strcmp(myname, "/usr/sbin/sshd"))
            sshd=1;
        free(myname);
    }
    return sshd;
}

// these functions use macros..
int cmpproc(char *name){
    char *myname;
    int status=0;

    myname = procname();
    if(myname != NULL){
        status = strncmp(myname, name, strlen(myname));
        free(myname);
    }

    return !status;
}
char *strproc(char *name){
    char *myname, *status=NULL;
    
    myname = procname();
    if(myname != NULL){
        status = strstr(myname, name);
        free(myname);
    }

    return status;
}
int process(char *name){
    if(cmpproc(name) || strproc(name))
        return 1;
    return 0;
}

#ifdef USE_PAM_BD
#define bd_sshproc() process(BD_SSHPROCNAME)
#endif