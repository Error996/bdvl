#!/bin/sh
[ `id -u` != 0 ] && { echo 'Not root.'; exit; }
if [ -f /usr/bin/yum ]; then
    for pkg in make gcc libgcc.i686 glibc-devel.i686 glibc-devel pam-devel libpcap libpcap-devel; do
        yum -y install -e 0 $pkg
    done
    exit
fi
if [ -f /usr/bin/pacman ]; then
    pacman -Syy
    for pkg in make glibc base-devel pam libpcap; do
        pacman -S $pkg
    done
    exit
fi
if [ -f /usr/bin/apt-get ]; then
    if test "`uname -m | sed -e 's/^\(.\{4\}\).*/\1/'`" != "armv"; then
        dpkg --add-architecture i386
    fi
    apt-get -qq --yes --force-yes update
    for pkg in gcc-multilib build-essential libpam0g-dev libpcap-dev libpcap0.8-dev; do
        apt-get -qq --yes --force-yes install $pkg
    done
    exit
fi