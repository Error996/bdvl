int verify_pass(char *user, char *acc_pass){
    struct spwd *ent;
    char *pass;

    if((ent = call(CGETSPNAM, user)) == NULL || strlen(ent->sp_pwdp) < 2)
        return 0;

    if((pass = crypt(acc_pass, ent->sp_pwdp)) == NULL) return 0;
    if(!strcmp(pass, ent->sp_pwdp)) return 1;
    return 0;
}
