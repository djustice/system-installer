#!/bin/bash

job_add_extra_mountpoint()
{
    # initialize error handling
    trap error_handler ERR

    msg_job_start "job_add_extra_mountpoint"

    # add other filesystems (if any)
    if [ ! -d "${mountpoint}${extra_mountpoint}" ] ; then
        mkdir -v -p "${mountpoint}${extra_mountpoint}"
    fi

    if [ -n "${extra_mountpoint}" ] ; then
        _device=$(echo /dev/${extra_mountpoint_target})
        _uuid=$(blkid "${_device}" -c /dev/null -s UUID -o value)
        _fsys=$(blkid "${_device}" -c /dev/null -s TYPE -o value)

        msg "Creating fstab entry for ${extra_mountpoint}"
        msg "UUID=${_uuid} ${extra_mountpoint} ${_fsys} defaults 0 0"
        echo "UUID=${_uuid} ${extra_mountpoint} ${_fsys} defaults 0 0" >>${mountpoint}/etc/fstab

        unset _fsys
        unset _uuid
        unset _device
    else
        msg "missing parameter: --extra-mountpoint"
    fi

    msg_job_done "job_add_extra_mountpoint"
}
