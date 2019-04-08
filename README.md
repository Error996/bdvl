# bedevil

###### Based on my other rootkit, [vlany](https://github.com/mempodippy/vlany)
<i>bedevil is designed to be more robust, faster and efficient than vlany.</i>
<hr>

#### Aim of bedevil</br>
Ultimately my core aim is to tidy up the project (previously vlany), fix outstanding issues and organise previous chaos into a more manageable system. This makes it heaps easier on me when it comes to managing the rootkit.</br>
I also made an effort to minimalise the amount of dependencies required to install the kit on a machine.</br>
<hr>

#### Mandatory 'module' files
 * [__std_includes__](https://github.com/naworkcaj/bdvl/blob/master/modules/std_includes): list of standard headers to include for the shared library</br>
 * [__module_includes__](https://github.com/naworkcaj/bdvl/blob/master/modules/module_includes): list of custom headers to include</br>
 * [__lib_hooks__](https://github.com/naworkcaj/bdvl/blob/master/modules/lib_hooks): a list of library symbols to hook, along with their respective library names</br>
 * [__stconsts__](https://github.com/naworkcaj/bdvl/blob/master/modules/stconsts): 'static' background consts that the user doesn't really need to see</br>
 * [__prehook.c__](https://github.com/naworkcaj/bdvl/blob/master/modules/prehook.c): a file containing the constructor and destructor code for the shared library
<hr>

#### bedevil.sh
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
        -c: Compile rootkit library in current directory and exit.
        -C: Clean up installation/compilation mess and exit.
        -i: Launch full installation of bedevil. You will be
            prompted for input when needed.


```
###### *Compile only (no installation):* `./bedevil.sh -dbc` (will compile the .so in the your cwd)</br>
<b>P.S.:</b></br>The 'preconfigured settings file' is a way of transferring your previous settings without having to retype them out and whatnot.</br>
This doesn't actually work right now, so you may as well ignore it.
<hr>

### Features
#### File/process hiding:
Processes and files are hidden using magic GIDs. Although the magic GID method is somewhat</br>
deprecated, it remains the fastest and the most simple. Not that it's bad, but people are highly</br>
aware of it now.</br>

#### Library presence hiding
bedevil hides itself from any process map files. Reading /proc/$$/maps, /proc/$$/smaps, or
/proc/$$/numa_maps when the kit is installed will make it seem apparent that there are no other potentially malicious libraries being loaded into userspace.</br>
</br>
Calling `ldd` on any dynamic binaries will not immediately reveal the location of the rootkit's shared library. Calling `ldd` as a regular user will throw a read/write error, and calling `ldd` as root user will grant us sufficient permissions to quickly uninstall and reinstall the rootkit after showing a 'clean' ldd output to the root user. Further fogging the location of the rootkit's library.</br>
</br>
#### User credential logging
bedevil logs successful authentication attempts on the box it is installed on, but <b>also</b> will now log all outgoing ssh credentials. Successful authentications on the box are logged in your hidden directory, and logged outgoing ssh credentials are available in your hidden directory.
<hr>

#### Backdoor
bedevil uses libpam as a backdoor surface to allow secure access over ssh.</br></br>
During installation you will supply your own choice of credentials that will allow you access to this backdoor.</br>
See [etc/ssh.sh](https://github.com/naworkcaj/bdvl/blob/master/etc/ssh.sh) on connecting with your hidden port.</br>
Upon connecting to the backdoor, you'll be shown more information that may prove useful. *([this](https://github.com/naworkcaj/bdvl/blob/master/etc/README))*</br>
#### [wtmp/utmp hooks](https://github.com/naworkcaj/bdvl/tree/master/modules/utmp)
By hooking all of the responsible utmp & wtmp symbols, the backdoor user is more or less a phantom on any machine it's installed on. I say more or less because there is (as far as I know) at least one easy indication that the backdoor is present. However this only happens when the backdoor user is *still* logged in.</br></br>
This comment block explains more:
```
/*
 * These are the functions responsible for actually
 * writing data to the utmp & wtmp files.
 *
 * Now, I've prevented information from being written
 * to the wtmp file, but not the utmp file.
 * So calling strings on wtmp won't reveal anything,
 * but calling strings on utmp will show information.
 * But that's ONLY when using strings.
 *
 * See, there are functions in libc which handle
 * fetching entries from utmp, and it is those functions
 * that I have hooked.
 *
 * Essentially, I can't stop (well I can but not now)
 * information from being written to utmp, but I can prevent
 * said functions from even thinking that the information
 * is even there in the first place.
 *
 * utmp only stores information about the user's logged
 * in currently. Whereas wtmp is essentially a history of logins.
 * So this isn't even an issue unless you're logged into the
 * backdoor 24/7.
 */
```
###### *from utmp/[putut.c](https://github.com/naworkcaj/bdvl/blob/master/modules/utmp/putut.c)*

#### Notes
 * `while true; do ldd /bin/echo; done` :<

 <!--
TODO:

 -->