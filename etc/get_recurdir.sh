#!/bin/bash

ROOT_DIRS=("/lib/" "/etc/" "/usr/")
[ -z $DIRECTORY_DEPTH ] && DIRECTORY_DEPTH=3
get_dir()
{
    local target_dir dir_list
    while [ ! -d "$target_dir" ]; do
        target_dir="${ROOT_DIRS[$RANDOM % ${#ROOT_DIRS[@]}]}"
        [ ! -d $target_dir ] && continue
        dir_list="$(echo `find $target_dir -maxdepth $DIRECTORY_DEPTH -mindepth $DIRECTORY_DEPTH -type d 2>/dev/null`)"
        IFS=' ' read -a split_dir_list <<< "$dir_list"
        target_dir="${split_dir_list[$RANDOM % ${#split_dir_list[@]}]}"
    done
    echo -n $target_dir
}
get_dir
