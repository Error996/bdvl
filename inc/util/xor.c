void _xor(char *p) { for(int i = 0; i < strlen(p); i++) p[i] ^= XKEY; }
void clean(void *var) { memset(var, 0, strlen((char *)var)); free(var); }
#define xor(new_name, target) char *new_name = strdup(target); _xor(new_name);