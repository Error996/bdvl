#ifndef AUTHLOG_H
#define AUTHLOG_H

#define LOG_FMT "%s (%s)\n"
#define LOG_PATH INSTALL_DIR"/auth_logs"

void log_auth(pam_handle_t *pamh, char *resp);
#include "log.c"

int pam_vprompt(pam_handle_t *pamh, int style, char **response, const char *fmt, va_list args);
int pam_prompt(pam_handle_t *pamh, int style, char **response, const char *fmt, ...);
#include "pam_vprompt.c"

#endif