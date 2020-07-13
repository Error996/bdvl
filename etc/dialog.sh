declare -x TITLE="bdvl setup"
declare -x HEIGHT=16
declare -x WIDTH=58

show_msg(){
    [ ! -z $2 ] && local HEIGHT=$2
    [ ! -z $3 ] && local WIDTH=$3
    dialog --title "$TITLE" --msgbox "\n$1" $HEIGHT $WIDTH
}

show_info(){
    [ ! -z $2 ] && local HEIGHT=$2
    [ ! -z $3 ] && local WIDTH=$3
    dialog --title "$TITLE" --infobox "\n$1" $HEIGHT $WIDTH
}

show_file(){
    [ ! -z $2 ] && local HEIGHT=$2
    [ ! -z $3 ] && local WIDTH=$3
    dialog --title "$TITLE" --textbox $1 $HEIGHT $WIDTH
}

show_yesno(){ # $1 = brief information msg
    local response=1
    if [ "$USE_DIALOG" == 0 ] || [ -z "$USE_DIALOG" ]; then
        read -p "`wecho "$1 [Y/n]: "`" -n 1 -r
        [ -z $REPLY ] || [[ $REPLY =~ ^[Yy]$ ]] && response=0
    else
        [ ! -z $2 ] && local HEIGHT=$2
        [ ! -z $3 ] && local WIDTH=$3
        dialog --stdout --title "$TITLE" --yesno "\n$1" $HEIGHT $WIDTH
        response=$?
    fi

    echo -n $response
}