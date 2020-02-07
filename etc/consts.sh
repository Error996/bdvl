find_consts(){
    local const_locations header_dirs
    header_dirs=(`get_header_dirs`)

    for dir in ${header_dirs[@]}; do
        local consts_path=$dir/consts
        [ ! -f $consts_path ] && continue
        [[ "${const_locations[*]}" == *"$consts_path"* ]] && continue
        const_locations+=($consts_path)
    done

    echo -n "${const_locations[*]}"
}

write_consts(){ # $1 = header file path
    local consts cconsts const_name \
          const_value locations
    locations=(`find_consts`)

    for location in ${locations[@]}; do
        consts="`cat $location | grep -o '^[^#]*'`"

        while read -r line; do
            IFS=':' read -r const_name const_value <<< "$line" # seperate name & value on line

            # if a const is toggle-specific and not needed, don't write it
            const_value="`cut_toggle_str "$const_value"`"
            [ "$const_value" == "(skip)" ] && continue

            # i want to support predetermined user paths, with bash variables if they're available
            if [[ "$const_value" == *"/"* ]] && [[ "$const_value" != *"*"* ]]; then
                IFS='/' read -a path_array <<< "$const_value"
                for p in ${path_array[@]}; do
                    [[ "${p//_/}" =~ ^[A-Z]+$ ]] || continue

                    # replace variable name in path with the value of the variable
                    const_value="${const_value//$p/${!p}}"
                done
            fi

            # use already exported variable
            [ "$const_value" == "$const_name" ] && { \
                cconsts+="#define $const_name \"`xor "${!const_value}"`\"\n"; \
                continue; \
            }

            # static value supplied. use it.
            cconsts+="#define $const_name \"`xor "$const_value"`\"\n"
        done <<< "$consts"
    done

    printf "\n$cconsts\n#define XKEY $XKEY\n"
}