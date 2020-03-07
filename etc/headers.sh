find_header_paths(){
    [ ! -d $NEW_MDIR ] && { eecho "$NEW_MDIR does not exist, can't find header paths"; exit; }
    local ret symbol_includes \
          include_path toggle_name

    symbol_includes="`cat $NEW_MDIR/includes.h | grep '.*\.h'`"
    while read -r line; do
        include_path="$NEW_MDIR/`echo -n "$line" | awk -F'"' '{print $2}'`"
        toggle_name="`cut_toggle_str "$line"`"
        [ "$toggle_name" == "(skip)" ] && continue
        [ -f $include_path ] && ret+=($include_path)
    done <<< "$symbol_includes"
    echo -n "${ret[*]}"
}

get_header_dirs(){
    local header_locations header_dirs
    header_locations=(`find_header_paths`)
    for header in ${header_locations[@]}; do header_dirs+=("`dirname $header`"); done
    echo -n "${header_dirs[*]}"
}