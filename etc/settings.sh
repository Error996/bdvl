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
    local var var_name input is_string \
          range_hide_ports
    var="$1"

    [[ $var == *"\""* ]] && is_string=1
    [[ $var != *"\""* ]] && is_string=0

    [ $is_string == 1 ] && input="`random 'a-z' 8`" # random text
    [ $is_string == 0 ] && input=`random '1-9' 4` # random integer

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
    echo "#endif" >> $1
}

populate_new_placeholders(){
    cp -r $MDIR/ $NEW_MDIR/ || { eecho "Couldn't copy module directory"; exit; }

    setup_header $BDVL_H
    [ `toggle_enabled HIDE_PORTS` == "true" ] && get_hide_ports

    local var_placeholders settings
    necho "Getting variable placeholders and their new values"
    var_placeholders=(`find_var_placeholders`)
    for var in ${var_placeholders[@]}; do settings+=(`get_setting "$var"`); done

    output_creds

    necho "Overwriting old variable placeholders with new settings"
    for selem in ${settings[@]}; do overwrite_placeholder "$selem"; done
}