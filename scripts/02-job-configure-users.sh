#!/bin/bash

job_configure_users()
{
    msg_job_start "job_configure_users"

    trap 'api_fs_umount "${mountpoint}" 2>/dev/null' EXIT
    api_fs_mount "${mountpoint}"

    # user is added and set the password by Tribe
    # let's do some post user add jobs

    chroot ${mountpoint} /usr/bin/gpasswd -a ${user_name} users
    chroot ${mountpoint} /usr/bin/gpasswd -a ${user_name} audio
    chroot ${mountpoint} /usr/bin/gpasswd -a ${user_name} video
    chroot ${mountpoint} /usr/bin/gpasswd -a ${user_name} network
    chroot ${mountpoint} /usr/bin/gpasswd -a ${user_name} optical
    chroot ${mountpoint} /usr/bin/gpasswd -a ${user_name} floppy
    chroot ${mountpoint} /usr/bin/gpasswd -a ${user_name} lp
    chroot ${mountpoint} /usr/bin/gpasswd -a ${user_name} log
    chroot ${mountpoint} /usr/bin/gpasswd -a ${user_name} power
    chroot ${mountpoint} /usr/bin/gpasswd -a ${user_name} scanner
    chroot ${mountpoint} /usr/bin/gpasswd -a ${user_name} storage
    chroot ${mountpoint} /usr/bin/gpasswd -a ${user_name} locate

    # fix quirks
    if [ -e "${mountpoint}/home/${user_name}/10-keyboard.conf" ] ; then
        msg "clean 10-keyboard.conf"
        rm -v ${mountpoint}/home/${user_name}/10-keyboard.conf
    fi

    if [ -e "${mountpoint}/home/${user_name}/mirrorlist" ] ; then
        msg "clean mirrorlist"
        rm -v ${mountpoint}/home/${user_name}/mirrorlist
    fi

    msg "chown ${user_name} /home/${user_name}"
    chroot ${mountpoint} /bin/chown -v ${user_name}:users /home/${user_name}
    chroot ${mountpoint} /bin/chown -vR ${user_name}:users /home/${user_name}/Desktop
    msg "chown ${user_name} /home/${user_name}/.*"
    chroot ${mountpoint} /usr/bin/find /home/${user_name}/ -maxdepth 1 -name '.*' -exec /bin/chown -vR ${user_name}:users '{}' \;

    msg_job_done "job_configure_users"
}
