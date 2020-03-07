#!/bin/bash

declare -a array YUM_DEPS=("gcc" "newt" \
                           "libgcc.i686" "glibc-devel.i686" "glibc-devel" \
                           "vim-common")

declare -a array APT_DEPS=("gcc-multilib" "build-essential")

declare -a array PAC_DEPS=("glibc" "base-devel")

script_root="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"
source $script_root/util.sh
[ `id -u` != 0 ] && { eecho "Not root. Cannot continue..."; exit; }
source $script_root/toggles.sh

[ "`toggle_enabled HIDE_PORTS`" == "true" ] && { \
    YUM_DEPS+=("libpcap" "libpcap-devel"); \
    APT_DEPS+=("libpcap-dev"); \
    PAC_DEPS+=("libpcap"); \
}

[ "`toggle_enabled USE_PAM_BD`" == "true" ] && { \
    YUM_DEPS+=("pam-devel"); \
    APT_DEPS+=("libpam0g-dev"); \
    PAC_DEPS+=("pam"); \
}

necho "Installing dependencies"
[ -f /usr/bin/yum ] && yum install -y -q -e 0 ${YUM_DEPS[*]}
[ -f /usr/bin/pacman ] && pacman -Syy &>/dev/null && pacman -S --noconfirm ${PAC_DEPS[*]}
if [ -f /usr/bin/apt-get ]; then
    dpkg --add-architecture i386
    yes | apt-get update
    apt-get --yes --force-yes install ${APT_DEPS[*]}
    [ "`toggle_enabled HIDE_PORTS`" == "true" ] && \
        [ ! -z "$(apt-cache search libpcap0.8-dev)" ] && \
            apt-get --yes --force-yes install libpcap0.8-dev
    grep -i ubuntu /proc/version &>/dev/null && rm -f /etc/init/plymouth*
fi
secho "Finished installing dependencies"