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
