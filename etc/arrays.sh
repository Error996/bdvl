get_arrayname(){ # $1 = line
    IFS=':' read -r array_name array_elements <<< "$1"
    echo "$array_name"
}
get_arrayelems(){ # $1 = line
    IFS=':' read -r array_name array_elements <<< "$1"
    array_elements="`cut_toggle_str "$array_elements"`"
    echo "$array_elements"
}

get_hooks(){ # $1 = char_arrays file path
    local hooks location contents \
          array_name array_elements

    location=$1
    contents="`read_cfg $location`"

    while read -r line; do
        array_name="`get_arrayname "$line"`"
        array_elements="`get_arrayelems "$line"`"
        [ "$array_elements" == "(skip)" ] || \
        [[ $array_name != *"_calls"* ]] && continue # need an array of function names
        IFS=',' read -a split_array_elements <<< "$array_elements"
        for current_hook in ${split_array_elements[@]}; do hooks+=($current_hook); done
    done <<< "$contents"

    echo -n "${hooks[*]}"
}

find_char_arrays(){
    local arrays_locations header_dirs
    header_dirs=(`get_header_dirs`)

    for dir in ${header_dirs[@]}; do
        local arrays_path=$dir/char_arrays
        [[ "${arrays_locations[*]}" == *"$arrays_path"* ]] && continue
        [ ! -f $arrays_path ] && continue
        arrays_locations+=($arrays_path)
    done

    echo -n "${arrays_locations[*]}"
}

# build an array of char pointers and print it out.
#   first argument must be an option, whether or not you want to write a new variable
#   or an already defined const.
#   second argument must be the name of the array to be created.
#   third argument must be the array itself.
#   last argument must be the size of said array.
build_array(){
    local option nam arr carr asize
    option=$1; nam=$2; arr=$3
    asize="#define `echo -n ${nam} | awk '{print toupper($0)}'`_SIZE $4"
    carr="\n${asize}\nstatic char *${nam}[`echo -n ${asize} | awk '{print $2}'`] = {"
    for e in ${arr[@]}; do
        [ $option == "char" ] && carr+="\"`xor "$e"`\","
        [ $option == "const" ] && carr+="$e,"
    done
    echo -n "${carr::-1}};\n"
}

buildall_char_arrays(){ # $1 = char_arrays file path
    local contents char_arrays location \
          array_name array_elements

    location=$1
    contents="`read_cfg $location`"

    while read -r line; do
        array_name="`get_arrayname "$line"`"
        array_elements="`get_arrayelems "$line"`"
        [ "$array_elements" == "(skip)" ] && continue

        # sort elements into a list and build our char array
        IFS=',' read -a split_array_elements <<< "$array_elements"
        char_arrays+="`build_array char "$array_name" \
                                        "${split_array_elements[*]}" \
                                        ${#split_array_elements[*]}`"
    done <<< "$contents"

    echo -n "$char_arrays"
}

write_char_arrays(){
    local char_arrays locations hooks
    locations=(`find_char_arrays`)

    for location in ${locations[@]}; do
        char_arrays+="`buildall_char_arrays $location`"
        hooks+=(`get_hooks $location`)
    done

    char_arrays+="`build_array char "all" "${hooks[*]}" ${#hooks[*]}`"
    for i in "${!hooks[@]}"; do char_arrays+="#define C`echo ${hooks[i]} | awk '{print toupper($0)}'` $i\n"; done
    char_arrays+="syms symbols[ALL_SIZE];\n"

    printf "\n$char_arrays"
}