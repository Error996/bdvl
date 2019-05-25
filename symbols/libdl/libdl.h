#ifndef LIBDL_H
#define LIBDL_H
#include "dlsym.c"
#include "gsym.c"
#define HOOK(o_sym, csym) if(!o_sym) o_sym = get_sym(RTLD_NEXT, csym);
#include "dladdr.c"
#include "dlinfo.c"
#endif
