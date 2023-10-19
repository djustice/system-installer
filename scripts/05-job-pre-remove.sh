#!/bin/bash

job_pre_remove()
{
    # initialize error handling
    trap error_handler ERR

    msg_job_start "job_pre_remove"

    trap 'api_fs_umount "${mountpoint}" 2>/dev/null' EXIT
    api_fs_mount "${mountpoint}"

    # remove any db.lck
    chroot ${mountpoint} /bin/rm -f /var/lib/pacman/db.lck

    # remove live-specific packages
    msg "running pacman"
    for p in ${REMOVE_LIVE_PACKAGES}
    do
        if [[ -n "$(chroot ${mountpoint} /usr/bin/pacman -Qsq "^$p$")" ]]; then
            chroot ${mountpoint} /usr/bin/pacman -Rddn ${p} --noconfirm
        fi
    done

    msg_job_done "job_pre_remove"
}
