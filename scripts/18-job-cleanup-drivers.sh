#!/bin/bash

job_cleanup_drivers()
{
    # initialize error handling
    trap error_handler ERR

    msg_job_start "job_cleanup_drivers"

    trap 'api_fs_umount "${mountpoint}" 2>/dev/null' EXIT
    api_fs_mount "${mountpoint}"

    ###########################################################################
    # CLEANUP XORG DRIVERS
    ###########################################################################
    msg "cleaning up video drivers"

    # remove any db.lck
    chroot ${mountpoint} /bin/rm -f /var/lib/pacman/db.lck

    USED_MODULES=$(lsmod | cut -d' ' -f1)
    ALL_DRIVERS=$(pacman -r ${mountpoint} -Q | grep xf86-video | cut -d "-" -f 3 | cut -d " " -f 1)

    touch /tmp/used_drivers
    [[ $(echo "$USED_MODULES" | grep "^radeon$") ]]  && echo "ati"    >> /tmp/used_drivers
    [[ $(echo "$USED_MODULES" | grep "^i915$") ]]    && echo "intel"  >> /tmp/used_drivers
    [[ $(echo "$USED_MODULES" | grep "^nvidia$") ]]  && echo "nvidia" >> /tmp/used_drivers

    for driver in ${ALL_DRIVERS}
    do
        [[ $(echo "$USED_MODULES" | grep "^${driver}$") ]] && echo "${driver}" >> /tmp/used_drivers
    done

    # reload real used drivers
    USED_DRIVERS=$(cat /tmp/used_drivers)

    # display found drivers
    msg "configured driver: ${USED_DRIVERS}"
    msg "installed drivers: ${ALL_DRIVERS}"

    msg "remove used drivers and vesa from remove_drivers list"
    echo "${ALL_DRIVERS}" > /tmp/remove_drivers
    for udriver in ${USED_DRIVERS}
    do
        for driver in ${ALL_DRIVERS}
        do
            if [ "${driver}" = "${udriver}" ] ; then
                sed -i "/${driver}/d" /tmp/remove_drivers
            fi
        done
    done
    sed -i "/vesa/d" /tmp/remove_drivers

    msg "cleanup drivers"
    REMOVE_DRIVERS=$(cat /tmp/remove_drivers)
    if [[ -n "$USED_DRIVERS" ]]; then
        for rdriver in ${REMOVE_DRIVERS}
        do
            chroot ${mountpoint} /usr/bin/pacman -Rn xf86-video-${rdriver} --noconfirm
        done
        msg "remove any unneeded dri pkgs"
        # tmp fix, use pacman -Rnscu $(pacman -Qdtq) somewhere at the end later
        # grep errors out if it can't find anything > using sed instead of grep, 
        REMOVE_DRI=$(pacman -r ${mountpoint} -Qdtq | sed -n '/dri/ p')
        for rdri in ${REMOVE_DRI}
        do
            chroot ${mountpoint} /usr/bin/pacman -Rn ${rdri} --noconfirm
        done
    else
        msg "module not found > not removing any free drivers"
        msg "output of lsmod:"
        lsmod | sort
        msg "output of lsmod done"
    fi

    msg "video driver removal complete"


    ###########################################################################
    # CLEANUP INPUT DRIVERS
    ###########################################################################
    msg "cleaning up input drivers"

    ALL_IDRIVERS=`pacman -r ${mountpoint} -Q | grep xf86-input | cut -d "-" -f 3 | cut -d " " -f 1 | grep -v keyboard | grep -v evdev | grep -vw mouse`


    #check for synaptics driver
    if cat /var/log/Xorg.0.log | grep "synaptics" > 0 ; then
        ALL_IDRIVERS=`echo $ALL_IDRIVERS | sed s/"synaptics"/""/g`
    fi
    #check for wacom driver
    if cat /var/log/Xorg.0.log | grep "wacom" > 0 ; then
        ALL_IDRIVERS=`echo $ALL_IDRIVERS | sed s/"wacom"/""/g`
    fi

    for driver in ${ALL_IDRIVERS}
    do
        chroot ${mountpoint} /usr/bin/pacman -Rnc xf86-input-${driver} --noconfirm
    done

    msg "input driver removal complete"

    msg_job_done "job_cleanup_drivers"
}
