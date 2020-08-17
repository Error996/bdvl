void gidchanger(void){
    if(getgid()==readgid() || rkprocup())
        return;

    int curtime = time(NULL);
    if(itistime(GIDTIME_PATH, curtime, GID_CHANGE_MINTIME))
        changerkgid(curtime);
}