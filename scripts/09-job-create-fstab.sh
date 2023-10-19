#!/bin/bash

job_create_fstab()
{
    # initialize error handling
    trap error_handler ERR

    msg_job_start "job_create_fstab"

    #
    # create fresh fstab on target
    #
    # at first clean fstab first from all current /dev and UUID entries
    #
    msg "Cleaning target fstab at first"
    sed -i -e '/^\/dev/d' ${mountpoint}/etc/fstab
    sed -i -e '/^UUID/d' ${mountpoint}/etc/fstab

    # add root filesystem
    _device=$(echo /dev/${target_root})
    _uuid=$(blkid "${_device}" -c /dev/null -s UUID -o value)
    _fsys=$(blkid "${_device}" -c /dev/null -s TYPE -o value)

    msg "Creating fstab entry for /"
    msg "UUID=${_uuid} / ${_fsys} defaults 0 1"
    echo "UUID=${_uuid} / ${_fsys} defaults 0 1" >>${mountpoint}/etc/fstab

    unset _fsys
    unset _uuid
    unset _device

    # add swap filesystem
    if [ "${use_swap}" = "yes" ] ; then
        _device=$(echo /dev/${target_swap})
        _uuid=$(blkid "${_device}" -c /dev/null -s UUID -o value)
        _fsys=$(blkid "${_device}" -c /dev/null -s TYPE -o value)

        msg "Creating fstab entry for swap"
        if [ "${_fsys}" != "swap" ] ; then
            msg "ERROR: wrong device supplied for creating the swap entry"
            msg "DEVICE=${_device} UUID=${_uuid} TYPE=${_fsys}"
        else
            msg "UUID=${_uuid} swap swap defaults 0 0"
            echo "UUID=${_uuid} swap swap defaults 0 0" >>${mountpoint}/etc/fstab
        fi

        unset _fsys
        unset _uuid
        unset _device
    else
        msg "Not using a swap partition"
    fi

    msg_job_done "job_create_fstab"
}
