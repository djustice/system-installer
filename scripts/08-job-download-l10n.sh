#!/bin/bash

job_download_l10n()
{
    msg_job_start "job_download_l10n"

    # remove any db.lck
    chroot ${mountpoint} /bin/rm -f /var/lib/pacman/db.lck

    if [ "${kdelang}" != "" ] ; then

        if [ "${ping_check}" != "" ] ; then
            pkg_installed=`pacman -Q --noconfirm | grep ${KDE_L10N_PREFIX}-${kdelang}`
            pkg_check=`pacman -Ss ${KDE_L10N_PREFIX}-${kdelang} --noconfirm | grep ${KDE_L10N_PREFIX}-${kdelang}`
            kdever_local=`pacman -Qi kde-common --noconfirm | sed -n 2p | cut -d':' -f 2 | cut -d' ' -f 2 | cut -d'-' -f 1`
            kdever_remote=`pacman -Ss kde-common --noconfirm | sed -n 1p | cut -d' ' -f 2 | cut -d'-' -f 1`
            if [ "${pkg_check}" = "" ] || [ "${pkg_installed}" != "" ] || [ "${kdever_local}" != "${kdever_remote}" ] ; then
                msg "no ${KDE_L10N_PREFIX}-${kdelang} available or already installed. skipping"
            else
                msg "installing ${KDE_L10N_PREFIX}-${kdelang}"
                /usr/bin/pacman -r ${mountpoint} -Syf ${KDE_L10N_PREFIX}-${kdelang} --noconfirm
            fi
        else
            msg "no internet connection available. skipping"
        fi

    else

        msg "l10n download disabled"
    fi

    msg_job_done "job_download_l10n"
}
