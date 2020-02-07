#ifndef UTMP_H
#define UTMP_H

static int hide_me = 0;

struct utmp *getutent(void);
struct utmpx *getutxent(void);
struct utmp *getutid(const struct utmp *ut);
struct utmpx *getutxid(const struct utmpx *utx);
void getutmp(const struct utmpx *ux, struct utmp *ut);
void getutmpx(const struct utmp *ut, struct utmpx *utx);
struct utmp *getutline(const struct utmp *ut);
struct utmpx *getutxline(const struct utmpx *utx);
#include "getut.c"
void logwtmp(const char *ut_line, const char *ut_name, const char *ut_host);
void updwtmp(const char *wfile, const struct utmp *ut);
void updwtmpx(const char *wfilex, const struct utmpx *utx);
#include "putut.c"
#endif
