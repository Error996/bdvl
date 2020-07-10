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

//#define PATH_MAX 4096
#define LINE_MAX 2048

typedef struct symbol_struct {
    void *(*func)();
} syms;

#define sizeofarray(arr) sizeof(arr) / sizeof(arr[0])