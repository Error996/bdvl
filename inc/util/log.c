int alreadylogged(const char *logpath, char *logbuf){
    FILE *fp;
    char line[LINE_MAX];
    int logged = 0;

    hook(CFOPEN);

    fp = call(CFOPEN, logpath, "r");
    if(fp == NULL && errno == ENOENT)
        return 0; // create the file..
    else if(fp == NULL) return 1;

    while(fgets(line, sizeof(line), fp) != NULL){
        if(!strcmp(line, logbuf)){
            logged = 1;
            break;
        }
    }

    fclose(fp);
    return logged;
}