#ifndef PAM_H
#define PAM_H
#include "pam/pam_private.h"

int pam_authenticate(pam_handle_t *pamh, int flags);
int pam_open_session(pam_handle_t *pamh, int flags);
int pam_acct_mgmt(pam_handle_t *pamh, int flags);
#include "pam/pam_hooks.c"

void pam_syslog(const pam_handle_t *pamh, int priority, const char *fmt, ...);
void pam_vsyslog(const pam_handle_t *pamh, int priority, const char *fmt, va_list args);
#include "pam/pam_syslog.c"

#define PAM_PORT ??PAM_PORT??  /* when set, is written to hide ports file */
#endif
