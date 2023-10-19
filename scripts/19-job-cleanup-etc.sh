#!/bin/bash

job_cleanup_etc()
{
    # initialize error handling
    trap error_handler ERR

    msg_job_start "job_cleanup_etc"

    msg "removing superfluous files from target"

    if [ -e "${mountpoint}/etc/inittab.larchsave" ] ; then
        rm -v -rf ${mountpoint}/etc/inittab.larchsave
    fi

    if [ -e "${mountpoint}/usr/share/applications/avahi-discover.desktop" ] ; then
        echo "OnlyShowIn=GNOME;" >> ${mountpoint}/usr/share/applications/avahi-discover.desktop
    fi

    if [ -e "${mountpoint}/usr/share/applications/bssh.desktop" ] ; then
        echo "OnlyShowIn=GNOME;" >> ${mountpoint}/usr/share/applications/bssh.desktop
    fi

    if [ -e "${mountpoint}/usr/share/applications/bvnc.desktop" ] ; then
        echo "OnlyShowIn=GNOME;" >> ${mountpoint}/usr/share/applications/bvnc.desktop
    fi

    msg "cleanup complete"

    msg_job_done "job_cleanup_etc"
}
