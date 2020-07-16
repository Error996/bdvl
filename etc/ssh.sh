#!/bin/sh
[ -z $SSH_PORT ] && SSH_PORT=22
usage(){ echo "usage: $0 [username] [host] [hidden port]"; exit; }
[ ! -f `which socat 2>/dev/null || echo "no"` ] && { echo "socat not installed :<"; exit; }
[ -z $1 ] || [ -z $2 ] || [ -z $3 ] && usage
echo "connecting to $1@$2 from port $3."
ssh -t -o ProxyCommand="socat - tcp4-connect:$2:$SSH_PORT,bind=:$3,reuseaddr" $2 -l "$1"
