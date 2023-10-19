#!/bin/bash

job_cleanup_l10n()
{
    # initialize error handling
    trap error_handler ERR

    msg_job_start "job_cleanup_l10n"

    trap 'api_fs_umount "${mountpoint}" 2>/dev/null' EXIT
    api_fs_mount "${mountpoint}"

    # remove any db.lck
    chroot ${mountpoint} /bin/rm -f /var/lib/pacman/db.lck

    #
    # CLEANUP UP LOCALIZATION
    #
    USED_L10N=${kdelang}
    ALL_L10N=$(pacman -r ${mountpoint} -Q | grep ${KDE_L10N_PREFIX} | cut -d " " -f 1 | awk -F "l10n-" '{print $2}')

    msg "configured localization: ${USED_L10N}"
    msg "installed localization(s): ${ALL_L10N}"

    if [[ "${netinst}" == "yes" ]]; then
        if [ "${kdelang}" != "" ] ; then
            pacman -r ${mountpoint} -S ${KDE_L10N_PREFIX}-${USED_L10N} --noconfirm
        else
            msg "l10n install disabled"
        fi
    else
        for l10npkg in ${ALL_L10N}
        do
            if [ "${l10npkg}" != "$USED_L10N" ] ; then
                pacman -r ${mountpoint} -Rddn kde-l10n-${l10npkg} --noconfirm
            fi
        done
    fi

    msg "l10n removal complete"

    msg_job_done "job_cleanup_l10n"
}
