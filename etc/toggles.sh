script_root="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"
source $script_root/util.sh
toggles_file="$script_root/../inc/toggles.h"
blacklist=("TOGGLES_H" "#endif" "//ignore")

cut_toggle_str(){ # $1 = line
    # find status of toggle on said line. indicate we are to skip writing line
    # if toggle is disabled. return the original line (with the toggle label
    # stripped) if otherwise.
    local toggle_name str_value
    str_value="$1"
    toggle_name="`get_toggle_name "$str_value"`"

    if [[ $toggle_name =~ [A-Z] ]]; then
        [ "`toggle_enabled "$toggle_name"`" == "false" ] && { echo -n "(skip)"; return; }
        str_value="${str_value//" [$toggle_name]"/}"
    fi

    echo -n "$str_value"
}

get_toggle_line(){ # $1 = toggle name
    local toggle_line
    toggle_line="`cat $toggles_file | grep "#.*$1"`"
    echo -n "$toggle_line"
}

get_toggle_name(){ # $1 = const line
    local line="$1"
    local toggle_name="`echo -n "$line" | awk -F" " '{print $NF}'`"
    toggle_name="${toggle_name//\[/}"
    toggle_name="${toggle_name//\]/}"
    echo -n $toggle_name
}

toggle_enabled(){ # $1 = toggle name
    local toggle_line toggle_status ret
    toggle_line="`get_toggle_line $1`"
    toggle_status="`echo -n "$toggle_line" | awk -F" " '{print $1}'`"
    [ "$toggle_status" == "#undef" ] && ret="false"
    [ "$toggle_status" == "#define" ] && ret="true"
    echo -n $ret
}

switch_toggle(){ # $1 = name
    local toggle_line toggle_name
    toggle_name=$1
    toggle_line="`get_toggle_line $toggle_name`"
    [ "`toggle_enabled $toggle_name`" == "false" ] && sed -i "s:$toggle_line:#define $toggle_name:" $toggles_file
    [ "`toggle_enabled $toggle_name`" == "true" ] && sed -i "s:$toggle_line:#undef $toggle_name:" $toggles_file
}

read_toggles(){
    local toggles_contents current_toggle_name skip toggles
    toggles_contents="`cat $toggles_file`"

    while read -r line; do
        skip=0
        for b in ${blacklist[@]}; do [[ "$line" == *"$b"* ]] && skip=1; done
        [ $skip == 1 ] || [[ "$line" != "#"* ]] && continue
        current_toggle_name="`echo -n "$line" | awk -F' ' '{print $2}'`"
        toggles+=($current_toggle_name)
    done <<< "$toggles_contents"

    echo -n "${toggles[*]}"
}

set_toggles(){ # $1 = toggle name
    local toggles status
    toggles=(`read_toggles`)
    necho "Decide what you want to enable/disable."
    for toggle in ${toggles[@]}; do
        status="`toggle_enabled $toggle`"
        read -p "`secho "$toggle = $status. Switch? [Y/n]: "`" -n 1 -r
        [ -z $REPLY ] && { switch_toggle $toggle; continue; }
        [[ $REPLY =~ ^[Yy]$ ]] && switch_toggle $toggle
        echo
    done
}