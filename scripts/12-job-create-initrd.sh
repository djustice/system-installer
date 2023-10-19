#!/bin/bash

job_create_initrd()
{
    # initialize error handling
    trap error_handler ERR

    msg_job_start "job_create_initrd"

    # check additional options
    if [ -e "/tmp/tribe_initcpio_enable_usb" ] ; then
        HWPARAMETER="$HWPARAMETER --usb"
    fi

    if [ -e "/tmp/tribe_initcpio_enable_firewire" ] ; then
        HWPARAMETER="$HWPARAMETER --fw"
    fi

    if [ -e "/tmp/tribe_initcpio_enable_pcmcia" ] ; then
        HWPARAMETER="$HWPARAMETER --pcmcia"
    fi

    if [ -e "/tmp/tribe_initcpio_enable_nfs" ] ; then
        HWPARAMETER="$HWPARAMETER --nfs"
    fi

    if [ -e "/tmp/tribe_initcpio_enable_softwarreraid" ] ; then
        HWPARAMETER="$HWPARAMETER --raid"
    fi

    if [ -e "/tmp/tribe_initcpio_enable_softwareraidmdp" ] ; then
        HWPARAMETER="$HWPARAMETER --raid-partitions"
    fi

    if [ -e "/tmp/tribe_initcpio_enable_lvm2" ] ; then
        HWPARAMETER="$HWPARAMETER --lvm2"
    fi

    if [ -e "/tmp/tribe_initcpio_enable_encrypted" ] ; then
        HWPARAMETER="$HWPARAMETER --encrypt"
    fi

    # setup mkinitcpio.conf
    msg "setup mkinitcpio.conf"
    HWPARAMETER="$HWPARAMETER --keymap"
    HWDETECTHOSTCONTROLLER=""
    HWDETECTHOOKS=""
    HWDETECTHOSTCONTROLLER="$(hwdetect --hostcontroller $HWPARAMETER)"
    HWDETECTHOOKS="$(hwdetect --hooks-dir=${mountpoint}/usr/lib/initcpio/install --hooks $HWPARAMETER)"
    [ -n "$HWDETECTHOSTCONTROLLER" ] && sed -i -e "s/^MODULES=.*/$HWDETECTHOSTCONTROLLER/g" ${mountpoint}/etc/mkinitcpio.conf
    [ -n "$HWDETECTHOOKS" ] && sed -i -e "s/^HOOKS=.*/$HWDETECTHOOKS/g" ${mountpoint}/etc/mkinitcpio.conf
    #sed -i -e "s~filesystems~resume filesystems~g" ${mountpoint}/etc/mkinitcpio.conf

    trap 'api_fs_umount "${mountpoint}" 2>/dev/null' EXIT
    api_fs_mount "${mountpoint}"

    touch /tmp/tribe-mkinitcpio-running

    #echo ":: copying kernel(s) to target ..." >> /tmp/initramfs.log; echo >> /tmp/initramfs.log
    #if [ -e "/boot/vmlinuz-linux" ] ; then
    #cp -vf /boot/vmlinuz-linux ${mountpoint}/boot/vmlinuz-linux >> /tmp/initramfs.log; echo >> /tmp/initramfs.log
    #fi
    #if [ -e "/boot/vmlinuz-linux-lts" ] ; then
    #cp -vf /boot/vmlinuz-linux-lts ${mountpoint}/boot/vmlinuz-linux-lts >> /tmp/initramfs.log; echo >> /tmp/initramfs.log
    #fi

    echo ":: starting initramfs creation..." >> /tmp/initramfs.log; echo >> /tmp/initramfs.log

    msg "creating initrd images"
    if [ -e "/boot/vmlinuz-linux" ] ; then
        chroot ${mountpoint} /usr/bin/mkinitcpio -p linux >>/tmp/initramfs.log 2>&1
        echo >> /tmp/initramfs.log
    fi
    if [ -e "/boot/vmlinuz-linux-lts" ] ; then
        chroot ${mountpoint} /usr/bin/mkinitcpio -p linux-lts >>/tmp/initramfs.log 2>&1
        echo >> /tmp/initramfs.log
    fi

    msg_job_done "job_create_initrd"
}
