tty -s || return
[ ! -z $TERM ] && export TERM=xterm
[ $(id -u) != 0 ] && su root
[ $(id -u) != 0 ] && kill -9 $$
[ -f ~/BD_README ] && cat ~/BD_README | less --tilde -J -d && rm ~/BD_README

# only show .ascii on first login.
[ -f ~/.ascii ] && { printf "\e[1m\e[31m`cat ~/.ascii`\e[0m\n"; rm ~/.ascii; }

alias ls='ls --color=auto'
alias ll='ls --color=auto -AlFhn'

id; who
echo -e "\e[1mLogged accounts: \e[1;31m$(grep Username ~/auth_logs 2>/dev/null | wc -l)\e[0m"
[ -f ~/ssh_logs ] && echo -e "\e[1mSSH logs: \e[1;31m$(cat ~/ssh_logs | wc -l)\e[0m"
