/*
*   DON'T TOUCH ANYTHING IN THIS FILE.
*/

//#define PATH_MAX 4096
#define LINE_MAX 2048

typedef struct {
    void *(*func)();
} syms;

#define sizeofarr(arr) sizeof(arr) / sizeof(arr[0])

