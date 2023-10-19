#!/bin/bash

job_setup_hardware()
{

    msg_job_start "job_setup_hardware"

    # remove any db.lck
    /bin/rm -f ${mountpoint}/var/lib/pacman/db.lck

    # setup alsa volume levels
    msg "setup alsa config"
    if [ -e "/etc/asound.state" ] ; then
        cp -v -a -f /etc/asound.state ${mountpoint}/etc/asound.state
    fi

    # alsa blacklist for the pc speaker
    if [ -e "/etc/modprobe.d/alsa_blacklist.conf" ] ; then
        cp -v -a -f /etc/modprobe.d/alsa_blacklist.conf \
            ${mountpoint}/etc/modprobe.d/alsa_blacklist.conf
    fi

    # blacklist for broken realtek nics
    if [ -e "/etc/modprobe.d/realtek_blacklist.conf" ] ; then
        cp -v -a -f /etc/modprobe.d/realtek_blacklist.conf \
            ${mountpoint}/etc/modprobe.d/realtek_blacklist.conf
    fi

    # setup proprietary drivers, if detected
    msg "setup proprietary drivers"
    if [ -e "/tmp/nvidia" ] ; then
        msg "nvidia detected"

        msg "removing unneeded packages"
        /usr/bin/pacman -r ${mountpoint} -Rdd --noconfirm \
            mesa-libgl xf86-video-nv xf86-video-nouveau
        msg "installing driver"
        if [[ "${netinst}" == "yes" ]]; then
            /usr/bin/pacman -r ${mountpoint} -S --noconfirm \
                nvidia-libgl nvidia-utils nvidia
        else
            /usr/bin/pacman -r ${mountpoint} -Udd --force --noconfirm \
                ${PKG_OVERLAY}/nvidia-libgl-343* \
                ${PKG_OVERLAY}/nvidia-343* \
                ${PKG_OVERLAY}/nvidia-utils-343*
        fi

    elif [ -e "/tmp/nvidia-340xx" ] ; then
        msg "nvidia-340xx detected"

        msg "removing unneeded packages"
        /usr/bin/pacman -r ${mountpoint} -Rdd --noconfirm \
            mesa-libgl \
            xf86-video-nv \
            xf86-video-nouveau
        msg "installing driver"
        if [[ "${netinst}" == "yes" ]]; then
            /usr/bin/pacman -r ${mountpoint} -S --noconfirm \
                nvidia-340xx-libgl nvidia-340xx nvidia-340xx-utils
        else
            /usr/bin/pacman -r ${mountpoint} -Udd --force --noconfirm \
                ${PKG_OVERLAY}/nvidia-340xx*
        fi

    elif [ -e "/tmp/nvidia-304xx" ] ; then
        msg "nvidia-304xx detected"

        msg "removing unneeded packages"
        /usr/bin/pacman -r ${mountpoint} -Rdd --noconfirm \
            mesa-libgl xf86-video-nv xf86-video-nouveau
        msg "installing driver"
        if [[ "${netinst}" == "yes" ]]; then
            /usr/bin/pacman -r ${mountpoint} -S --noconfirm \
                nvidia-304xx-libgl nvidia-304xx nvidia-304xx-utils
        else
            /usr/bin/pacman -r ${mountpoint} -Udd --force --noconfirm \
                ${PKG_OVERLAY}/nvidia-304xx*
        fi

    elif [ -e "/tmp/nvidia-173xx" ] ; then
        msg "nvidia-173xx detected"

        msg "removing unneeded packages"
        /usr/bin/pacman -r ${mountpoint} -Rdd --noconfirm \
            mesa-libgl xf86-video-nv xf86-video-nouveau
        msg "installing driver"
        if [[ "${netinst}" == "yes" ]]; then
            /usr/bin/pacman -r ${mountpoint} -S --noconfirm \
                nvidia-173xx nvidia-173-utils
        else
            /usr/bin/pacman -r ${mountpoint} -Udd --force --noconfirm \
                ${PKG_OVERLAY}/nvidia-173xx*
        fi

    elif [ -e "/tmp/catalyst" ] ; then
        msg "catalyst detected"

        msg "removing unneeded packages"
        # remove conflicting packages
        /usr/bin/pacman -r ${mountpoint} -Rdd --noconfirm \
            mesa-libgl xf86-video-ati

        msg "installing driver"
        # install driver
        if [[ "${netinst}" == "yes" ]]; then
            /usr/bin/pacman -r ${mountpoint} -S --noconfirm \
                catalyst-libgl catalyst catalyst-utils
        else
            /usr/bin/pacman -r ${mountpoint} -Udd --force --noconfirm \
                ${PKG_OVERLAY}/catalyst-libgl* \
                ${PKG_OVERLAY}/catalyst-utils* \
                ${PKG_OVERLAY}/catalyst-1*
        fi

    fi

    # fixing alsa
    alsactl -f ${mountpoint}/var/lib/alsa/asound.state store

    msg_job_done "job_setup_hardware"
}
