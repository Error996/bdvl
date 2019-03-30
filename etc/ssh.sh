#!/bin/bash
usage() { echo "usage: $0 [username] [host] [hidden port]"; exit; }
[ ! -f /usr/bin/socat ] && { echo "socat not installed :<"; exit; }
[ -z $1 ] && usage; # no username?
[ -z $2 ] && usage; # no host?
[ -z $3 ] && usage; # no hidden port?
echo "connecting to $1@$2:$3" && read -p "press enter to continue"
ssh -o ProxyCommand="socat - tcp4-connect:$2:22,bind=:$3" $2 -l "$1"