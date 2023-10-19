#!/bin/bash

job_initialize_target()
{
    # initialize error handling
    trap error_handler ERR

    msg_job_start "job_initialize_target"

    msg "initialize special directories"
    rm -v -rf ${mountpoint}/sys ${mountpoint}/proc ${mountpoint}/dev
    msg $(ls ${mountpoint})
    mkdir -p -v -m 1777 ${mountpoint}/tmp
    mkdir -p -v -m 1777 ${mountpoint}/var/tmp
    mkdir -p -v ${mountpoint}/boot/grub
    mkdir -p -v ${mountpoint}/usr/lib/locale
    mkdir -p -v ${mountpoint}/usr/share/icons/default
    mkdir -p -v ${mountpoint}/media
    mkdir -p -v ${mountpoint}/mnt
    mkdir -v ${mountpoint}/sys
    mkdir -v ${mountpoint}/proc

    msg "create the basic devices (/dev/{console,null,zero}) on the target"
    mkdir -v ${mountpoint}/dev
    mknod ${mountpoint}/dev/console c 5 1
    mknod ${mountpoint}/dev/null c 1 3
    mknod ${mountpoint}/dev/zero c 1 5

    msg "adjust permissions on /tmp and /var/tmp"
    chmod -v 777 ${mountpoint}/var/tmp
    chmod -v o+t ${mountpoint}/var/tmp
    chmod -v 777 ${mountpoint}/tmp
    chmod -v o+t ${mountpoint}/tmp

    msg "install /etc/resolv.conf"
    cp -vf /etc/resolv.conf ${mountpoint}/etc/resolv.conf

    msg "install configs for root"
    cp -vf /etc/skel/.bashrc ${mountpoint}/root/.bashrc
    cp -vf /etc/skel/.bash_profile ${mountpoint}/root/.bash_profile
    cp -vf /etc/skel/.xinitrc ${mountpoint}/root/.xinitrc
    cp -vf /etc/skel/.xsession ${mountpoint}/root/.xsession

    msg "set mouse theme for root"
    cp -vf /usr/share/icons/default/index.theme ${mountpoint}/usr/share/icons/default/index.theme

    msg "configure kdmrc"
    sed -i -e "s~^.*Theme=/.*~Theme=/usr/share/apps/kdm/themes/${THEME}~" ${mountpoint}/usr/share/config/kdm/kdmrc
    sed -i -e 's~^.*#AntiAliasing=.*~AntiAliasing=true~' ${mountpoint}/usr/share/config/kdm/kdmrc
    sed -i -e 's~^.*#TerminateServer=.*~TerminateServer=true~' ${mountpoint}/usr/share/config/kdm/kdmrc
    sed -i -e 's~^.*#HaltCmd=.*~HaltCmd=/sbin/poweroff~' ${mountpoint}/usr/share/config/kdm/kdmrc
    sed -i -e 's~^.*#RebootCmd=.*~RebootCmd=/sbin/reboot~' ${mountpoint}/usr/share/config/kdm/kdmrc

    msg "copy theme settings"
    mkdir -p ${mountpoint}/root/.config
    cp -v /home/live/.config/Trolltech.conf ${mountpoint}/root/.config/Trolltech.conf

    msg "set Chakra Version"
    echo "DISTRIB_ID=\"Chakra\"" > "${mountpoint}/etc/lsb-release"
    echo "DISTRIB_RELEASE=\"chakra-version\"" >> "${mountpoint}/etc/lsb-release"
    echo "DISTRIB_CODENAME=\"code-name\"" >> "${mountpoint}/etc/lsb-release"
    echo "DISTRIB_DESCRIPTION=\"Chakra Linux\"" >> "${mountpoint}/etc/lsb-release"
    echo "Chakra Linux release chakra-version (code-name)" > "${mountpoint}/etc/chakra-release"
    echo "Chakra Linux (chakra-version - code-name) \r  (\n) (\l)" > "${mountpoint}/etc/issue"

    msg "fix hwclock"
    mkdir -pv ${mountpoint}/var/lib/hwclock

    msg_job_done "job_initialize_target"
}
