#define MAXLDS 10  // the maximum number of ld.so paths that can be found/stored. more than enough?

static char *const ldhomes[7] = {"/lib", "/lib32", "/lib64", "/libx32",
                                 "/lib/x86_64-linux-gnu", "/lib/i386-linux-gnu", "/lib/arm-linux-gnueabihf"};

char **ldfind(int *allf);
#include "find.c"

int _ldpatch(const char *path, const char *oldpreload, const char *newpreload);
int ldpatch(const char *oldpreload, const char *newpreload);
#include "patch.c"