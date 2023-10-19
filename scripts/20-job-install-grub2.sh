#!/bin/bash

job_install_grub2()
{
    trap error_handler ERR

    msg_job_start "job_install_grub2"

    trap 'api_fs_umount "${mountpoint}" 2>/dev/null' EXIT
    api_fs_mount "${mountpoint}"

    msg "installing grub"
    _device=$(echo ${target_boot} | cut -b1-3)
    echo "GRUB2___ >>>> mountpoint: ${mountpoint}"
    echo "GRUB2___ >>>> target_boot: ${target_boot}"
    echo "GRUB2___ >>>> device: ${_device}"
    chroot ${mountpoint} grub-install /dev/${_device} --no-floppy > /tmp/grub2.log 2>&1

    if [ -e "/tmp/catalyst" ] ; then
        msg "Adding nomodeset for catalyst to kernel line"
        sed -i -e "s,GRUB_CMDLINE_LINUX_DEFAULT=.*,GRUB_CMDLINE_LINUX_DEFAULT=\"quiet splash radeon.modeset=0\",g" ${mountpoint}/etc/default/grub
        sed -i -e "s,GRUB_CMDLINE_LINUX=.*,GRUB_CMDLINE_LINUX_DEFAULT=\"quiet splash radeon.modeset=0\",g" ${mountpoint}/etc/default/grub
    fi

    if [ -e "/tmp/nvidia" ] ; then
        msg "Nvidia has boot issues with quiet settings"
        sed -i -e "s,GRUB_CMDLINE_LINUX_DEFAULT=.*,GRUB_CMDLINE_LINUX_DEFAULT=\"\",g" ${mountpoint}/etc/default/grub
    fi

    # set grub theme
    msg "setting grub theme: Sirius"
    sed -i -e 's,.*GRUB_THEME=.*,GRUB_THEME="/usr/share/grub/themes/Sirius/theme.txt",' ${mountpoint}/etc/default/grub

    if [ "${use_swap}" = "yes" ] ; then
        _sdevice=$(echo /dev/${target_swap})
        _suuid=$(blkid "${_sdevice}" -c /dev/null -s UUID -o value)
        _sfsys=$(blkid "${_sdevice}" -c /dev/null -s TYPE -o value)

        msg "we are using swap partition"
        if [ "${_sfsys}" != "swap" ] ; then
            msg "ERROR: wrong device supplied for setting a resume device"
            msg "DEVICE=${_sdevice} UUID=${_suuid} TYPE=${_sfsys}" 
        else
            msg "adding resume=UUID=${_suuid} to grub.cfg"
            sed -i -e "s~filesystems~resume filesystems~g" ${mountpoint}/etc/mkinitcpio.conf
            chroot ${mountpoint} /usr/bin/mkinitcpio -p linux >>/tmp/initramfs.log 2>&1
            echo >> /tmp/initramfs_swap.log
            #sed -i -e "s~root=UUID=~resume=UUID=${_suuid} root=UUID=~g" ${mountpoint}/boot/grub/grub.cfg
            sed -i -e "s,GRUB_CMDLINE_LINUX="",GRUB_CMDLINE_LINUX="resume=UUID=${_suuid}",g" ${mountpoint}/etc/default/grub
        fi

        unset _sfsys
        unset _suuid
        unset _sdevice
    fi

    msg "generating []/boot/grub/grub.cfg"
    chroot ${mountpoint} grub-mkconfig -o /boot/grub/grub.cfg >> /tmp/grub2.log 2>&1

    msg_job_done "job_install_grub2"
}
