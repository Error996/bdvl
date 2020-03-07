#!/bin/bash
declare -x script_root="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"
declare -x BADC=0

source $script_root/util.sh

[ `id -u` != 0 ] && { \
    wecho "You do not have root privileges, your actions are limited."; \
    BADC=$(( BADC + 1 )); \
    NOT_ROOT=1; \
}

[ -e /proc ] || { \
    eecho "/proc doesn't exist. May I suggest bailing?"; \
    BADC=$(( BADC + 1 )); \
}

[ ! -f `bin_path gcc` ] && { \
    eecho "GCC not found"; \
    exit; \
}

[ -f /etc/ssh/sshd_config ] || { \
    wecho "/etc/ssh/sshd_config not present"; \
    BADC=$(( BADC + 1 )); \
}

[[ $(cat /etc/syslinux/config 2>/dev/null | grep "SELINUX=" | tail -n 1) == *"enforcing"* ]] && { \
    eecho "SELinux detected (enforcing)"; \
    BADC=$(( BADC + 1 )); \
}

[ -d /proc/xen ] && { \
    wecho "Xen environment detected"; \
    BADC=$(( BADC + 1 )); \
}

[ -d /proc/vz ] && { \
    wecho "OpenVZ environment detected"; \
    BADC=$(( BADC + 1 )); \
}

[ -f /usr/bin/lveps ] && { \
    wecho "CloudLinux LVE detected"; \
    BADC=$(( BADC + 1 )); \
}

[[ $(cat /proc/scsi/scsi 2>/dev/null | grep 'VBOX') == *"VBOX"* ]] && { \
    wecho "VirtualBox VM detected"; \
    BADC=$(( BADC + 1 )); \
}

wecho "$BADC potential issue(s) detected."
sleep 1