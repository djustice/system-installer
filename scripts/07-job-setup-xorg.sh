#!/bin/bash

job_setup_xorg()
{
    # initialize error handling
    trap error_handler ERR

    msg_job_start "job_setup_xorg"

    # copy generated xorg.xonf to target
    if [ -e "/etc/X11/xorg.conf" ] ; then
        msg "copying generated xorg.conf to target"
        cp -v /etc/X11/xorg.conf ${mountpoint}/etc/X11/xorg.conf
    fi

    msg "generating keyboard config into target"

    mkdir -p ${mountpoint}/etc/X11/xorg.conf.d &> /dev/null
    echo "Section \"InputClass\"" >> ${mountpoint}/etc/X11/xorg.conf.d/10-keyboard.conf
    echo "    Identifier             \"Keyboard Defaults\"" >> ${mountpoint}/etc/X11/xorg.conf.d/10-keyboard.conf
    echo "    MatchIsKeyboard        \"yes\"" >> ${mountpoint}/etc/X11/xorg.conf.d/10-keyboard.conf

    if [ -n "$kblayout" ] ; then 
        msg "detected keyboard layout: $kblayout"
    else
        kblayout="us"
        msg "keyboard layout NOT detected, defaulting to US"
    fi
    echo "    Option                 \"XkbLayout\" \"${kblayout}\"" >> ${mountpoint}/etc/X11/xorg.conf.d/10-keyboard.conf

    if [ -n "$kbvariant" ] ; then 
        msg "detected keyboard variant: $kbvariant"
        echo "    Option                 \"XkbVariant\" \"$kbvariant\"" >> ${mountpoint}/etc/X11/xorg.conf.d/10-keyboard.conf
    else
        msg "keyboard variant NOT detected, defaulting to none"
    fi

    echo "EndSection" >> ${mountpoint}/etc/X11/xorg.conf.d/10-keyboard.conf

    msg_job_done "job_setup_xorg"
}
