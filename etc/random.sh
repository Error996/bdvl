# my idea behind the use of random_path and random_name is extra obscurity.
# i.e. no one installation of bedevil will have the same pathnames etc
#      for newly created files/general paths.
# now instead of new & random paths being something like '/usr/lib/apt/.$RANDOM',
# they'll be something a bit more like '/usr/lib/apthu'

random(){
    local charset=$1
    local len=$2
    echo -n "`cat /dev/urandom | tr -dc $charset | fold -w $len | head -n 1`"
}

# gets a totally random & unused path on the box.
# does this by first getting a random (existing) path on the box and appending
# a couple of random characters to the end of the path.
random_path(){
    local root_dirs blacklist \
          depth target_dir dir_list

    # look among these directories for a suitable path
    root_dirs=("/lib/" "/etc/" "/usr/")

    # for whatever reason, don't return a random path with these directory names
    blacklist=("pacman.d" "X11")
    depth=$1

    while [ ! -d "$target_dir" ]; do
        # get a random dir from our list, if it doesn't exist, go again
        target_dir="${root_dirs[$RANDOM % ${#root_dirs[@]}]}"
        [ ! -d $target_dir ] && continue

        # find all directories $depth deep and sort them all into a list
        dir_list="$(echo `find $target_dir -maxdepth $depth -mindepth $depth -type d 2>/dev/null`)"
        IFS=' ' read -a split_dir_list <<< "$dir_list"

        # select a random directory from the new list. if the selected dir is blacklisted, go again
        target_dir="${split_dir_list[$RANDOM % ${#split_dir_list[@]}]}"
        for dir in ${blacklist[@]}; do [[ $target_dir == *"$dir"* ]] && target_dir=""; done
    done

    local rand_chrs=`random '1-3' 1`
    echo -n "$target_dir`random 'a-z' $rand_chrs`"
}

random_name(){
    local names name name_length
    names=(`read_cfg $script_root/names.txt`)
    name_length=6

    name=${names[$RANDOM % ${#names[@]}]}
    while [ ${#name} -gt $name_length ]; do name=${name::${#name}-1}; done
    while [ ${#name} -lt $name_length ]; do name+="`random 'a-z' 1`"; done

    echo -n "$name"
}