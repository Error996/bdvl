#ifndef SYSLOG_H
#define SYSLOG_H
void openlog(const char *ident, int option, int facility);
void syslog(int priority, const char *format, ...);
void __syslog_chk(int priority, int flag, const char *format, ...);
void vsyslog(int priority, const char *format, va_list ap);
#include "syslog.c"
#endif
