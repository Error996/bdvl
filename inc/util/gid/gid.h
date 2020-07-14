#ifndef _GID_H_
#define _GID_H_

#ifdef READ_GID_FROM_FILE
gid_t changerkgid(void);
#include "change.c"
#endif

gid_t readgid(void);
#include "readgid.c"

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