#!/bin/bash

script_root="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"
source $script_root/util.sh
source $script_root/toggles.sh

[ `id -u` != 0 ] && { eecho 'Not root. Cannot continue...'; exit; }

declare -a array YUM_DEPS=('gcc' 'newt' 'libgcc.i686'
                           'glibc-devel.i686' 'glibc-devel'
                           'vim-common')
declare -a array APT_DEPS=('gcc-multilib' 'build-essential')
declare -a array PAC_DEPS=('glibc' 'base-devel')

[ "`toggle_enabled HIDE_PORTS`" == 'true' ] && {
    YUM_DEPS+=('libpcap' 'libpcap-devel');
    APT_DEPS+=('libpcap-dev' 'libpcap0.8-dev');
    PAC_DEPS+=('libpcap');
}

[ "`toggle_enabled USE_PAM_BD`" == 'true' ] && {
    YUM_DEPS+=('pam-devel');
    APT_DEPS+=('libpam0g-dev');
    PAC_DEPS+=('pam');
}

necho 'Installing dependencies'
[ -f /usr/bin/yum ] && { for pkg in ${YUM_DEPS[@]}; do yum install -e 0 $pkg; done; }
[ -f /usr/bin/pacman ] && { pacman -Syy; for pkg in ${PAC_DEPS[@]}; do pacman -S $pkg; done; }
[ -f /usr/bin/pacman ] && pacman -Syy && pacman -S ${PAC_DEPS[*]}
if [ -f /usr/bin/apt-get ]; then
    [[ "`uname -m`" != 'armv'* ]] && dpkg --add-architecture i386
    apt-get --yes --force-yes update
    for pkg in ${APT_DEPS[@]}; do apt-get --yes --force-yes install $pkg; done
    grep -i ubuntu /proc/version &>/dev/null && rm -f /etc/init/plymouth*
fi

secho 'Finished installing dependencies'