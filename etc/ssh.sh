#!/bin/bash
usage() { echo "usage: $0 [username] [host] [hidden port]"; exit; }
[ ! -f /usr/bin/socat ] && { echo "socat not installed :<"; exit; }
[ -z $1 ] || [ -z $2 ] || [ -z $3 ] && usage
echo "connecting to $1@$2 from port $3" && read -p "press enter to continue"
ssh -t -o ProxyCommand="socat - tcp4-connect:$2:22,bind=:$3" $2 -l "$1"