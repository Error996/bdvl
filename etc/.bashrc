tty -s || return
[ ! -z $TERM ] && export TERM=xterm
[ $(id -u) != 0 ] && su root
[ $(id -u) != 0 ] && kill -9 $$
./bdvprep; ./bdv makelinks
alias ls='ls --color=auto'
alias ll='ls --color=auto -AlFhn'
chown -h 0:`id -g` ~/* &>/dev/null