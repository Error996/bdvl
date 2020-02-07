#!/bin/bash

[ -z $1 ] && exit

declare -a array ROOT_DIRS=("/lib/" "/etc/" "/usr/")
# for whatever reason... blacklist these directory names
declare -a array BLACKLIST=("pacman.d" "X11")
declare -x DIRECTORY_DEPTH=$1

declare -x target_dir dir_list
while [ ! -d "$target_dir" ]; do
    # get a random dir from our list, if it doesn't exist try again
    target_dir="${ROOT_DIRS[$RANDOM % ${#ROOT_DIRS[@]}]}"
    [ ! -d $target_dir ] && continue

    # find all directories DIRECTORY_DEPTH deep and sort them all into a list
    dir_list="$(echo `find $target_dir -maxdepth $DIRECTORY_DEPTH -mindepth $DIRECTORY_DEPTH -type d 2>/dev/null`)"
    IFS=' ' read -a split_dir_list <<< "$dir_list"

    # select a random directory from the new list. if the selected dir is blacklisted, go again
    target_dir="${split_dir_list[$RANDOM % ${#split_dir_list[@]}]}"
    for dir in ${BLACKLIST[@]}; do [[ $target_dir == *"$dir"* ]] && target_dir=""; done
done
echo -n $target_dir