#!/bin/bash
[ $(id -u) != 0 ] && { eecho "You do not have root privileges, your actions are limited"; NOT_ROOT=1; }
[ -e /proc ] || { eecho "/proc doesn't exist. May I suggest bailing?"; exit; }
[ ! -f `which gcc 2>/dev/null || echo "no"` ] && { eecho "GCC not found"; exit; }
[ -f /etc/ssh/sshd_config ] || { eecho "/etc/ssh/sshd_config not present"; exit; }
[[ $(cat /etc/syslinux/config 2>/dev/null | grep "SELINUX=" | tail -n 1) == *"enforcing"* ]] && { eecho "SELinux detected (enforcing)"; exit; }

[ -d /proc/xen ] && wecho "Xen environment detected"
[ -d /proc/vz ] && wecho "OpenVZ environment detected"
[ -f /usr/bin/lveps ] && wecho "CloudLinux LVE detected"
[[ $(cat /proc/scsi/scsi 2>/dev/null | grep 'VBOX') == *"VBOX"* ]] && wecho "VirtualBox VM detected"