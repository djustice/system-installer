#!/bin/bash

job_pre_install()
{
    # initialize error handling
    trap error_handler ERR

    msg_job_start "job_pre_install"

    trap 'api_fs_umount "${mountpoint}" 2>/dev/null' EXIT
    api_fs_mount "${mountpoint}"

    # remove any db.lck
    chroot ${mountpoint} /bin/rm -f /var/lib/pacman/db.lck

    msg "About to install on target: ${INSTALL_PACKAGES}"

    for p in ${INSTALL_PACKAGES}
    do
        msg "Installing ${p}"
        if [[ "${netinst}" == "yes" ]]; then
            pacman -r ${mountpoint} -S ${p} --noconfirm
        else
            pacman -r ${mountpoint} -Ud --force ${PKG_OVERLAY}/${p}-* --noconfirm
        fi
    done

    msg_job_done "job_pre_install"
}
