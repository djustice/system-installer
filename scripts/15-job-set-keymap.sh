#!/bin/bash

job_set_keymap()
{
    # initialize error handling
    trap error_handler ERR

    msg_job_start "job_set_keymap"

    #
    # this checks for the Tribe selected layout, else sets English
    #
    if [ -n "$kblayout" ] ; then 
        msg "detected $kblayout"
    else
        kblayout="us"
        msg "keyboard layout NOT detected, defaulting to US"
    fi

    if [ "$kbvariant" = "dvorak" ] ; then
        msg "detected dvorak variant"
        kblayout="dvorak"
    fi

    echo "KEYMAP=\"${kblayout}\"" >> ${mountpoint}/etc/vconsole.conf

    msg_job_done "job_set_keymap"
}
