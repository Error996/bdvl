# handles reading ports intended for part of a range
# from 'range_hide_ports[@]' (populated by 'add_hiddenport')
# and properly formats them into applicable ranges before
# adding them to 'hide_ports[@]' for the final write.
get_hideportranges(){
    for i in ${!range_hide_ports[@]}; do
        local tmp+="${range_hide_ports[$i]}-"
        if (( $i % 2 )); then
            hide_ports+=("${tmp::${#tmp}-1}")
            unset tmp
        fi
    done
}

write_hideports(){ # $1 = path to write to
    get_hideportranges

    local path=$1

    touch $path && chmod 644 $path
    for port in ${hide_ports[@]}; do echo "$port" >> $path; done
}

# just handles determining the differences between individual ports & ranges,
# and adds them to their special arrays.
add_hiddenport(){ # $1 = variable name, $2 = variable value
    local name value underscores

    name="$1"
    value="$2"

    # if the variable name has 2 underscores in it, it's part of a range.
    # i hate this.
    underscores=`echo -n "$name" | awk -F"_" '{printf NF-1}'`

    [ $underscores == 2 ] && range_hide_ports+=($value)
    [ $underscores == 1 ] && hide_ports+=($value)
}