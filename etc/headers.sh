# reads includes from includes.h and builds the full path for the
# current include before adding it to a list.
find_header_paths(){
    [ ! -d $NEW_MDIR ] && { \
        eecho "$NEW_MDIR does not exist, can't find header paths" && \
        exit; \
    }

    local ret symbol_includes \
          include_path toggle_name

    # get all of our included headers
    symbol_includes="`cat $NEW_MDIR/includes.h | grep '.*\.h'`"

    while read -r line; do
        # full path of current header. i use awk here basically just to eliminate
        # all quotation marks from the path.
        include_path="$NEW_MDIR/`echo -n "$line" | awk -F'"' '{print $2}'`"

        toggle_name="`cut_toggle_str "$line"`"        # here i'm just checking if we actually want
        [ "$toggle_name" == "(skip)" ] && continue    # to add the include to the list, should the
                                                      # header only be included conditionally based
                                                      # on the status of its toggle.

        # if we don't need to touch the current header as it doesn't
        # have any placeholders in it, don't add it to our list.
        [ -z "`cat $include_path 2>/dev/null | grep '??'`" ] && continue
        [ -f $include_path ] && ret+=($include_path)  # if all's good, add the header to our list.
    done <<< "$symbol_includes"

    echo -n "${ret[*]}"
}

# fetches all of the included header paths & returns a list
# of our header's directories. (used for reading all of the
# required 'arrays' & 'consts' files)
get_header_dirs(){
    local header_locations header_dirs
    header_locations=(`find_header_paths`)
    for header in ${header_locations[@]}; do header_dirs+=("`dirname $header`"); done
    echo -n "${header_dirs[*]}"
}