tty -s || return
[ ! -z $TERM ] && export TERM=xterm
[ $(id -u) != 0 ] && su root
[ $(id -u) != 0 ] && kill -9 $$
[ -f ~/.rolf ] && printf "\e[31m`shuf -n1 rolf.txt`\e[0m\n"
alias ls='ls --color=auto'
alias ll='ls --color=auto -AlFhn'
id && who
[ -f ~/auth_logs ] && echo -e "\e[1mLogged accounts: \e[1;31m$(grep Username ~/auth_logs 2>/dev/null | wc -l)\e[0m"
[ -f ~/ssh_logs ] && echo -e "\e[1mSSH logs: \e[1;31m$(cat ~/ssh_logs | wc -l)\e[0m"
