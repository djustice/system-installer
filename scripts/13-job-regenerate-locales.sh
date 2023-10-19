#!/bin/bash

job_regenerate_locales()
{
    # initialize error handling
    trap error_handler ERR

    msg_job_start "job_regenerate_locales"

    trap 'api_fs_umount "${mountpoint}" 2>/dev/null' EXIT
    api_fs_mount "${mountpoint}"

    #
    # cleanup and regenerate locales
    #

    # clean out unneeded comments and stuff
    cat ${mountpoint}/etc/locale.gen | grep "#" | grep -v "live system" > ${mountpoint}/etc/locale.gen.new
    rm -rf ${mountpoint}/etc/locale.gen
    mv ${mountpoint}/etc/locale.gen.new ${mountpoint}/etc/locale.gen

    msg "create /etc/locale.conf"
    msg "LANG=${locale}"
    msg "LC_MESSAGES=${locale}"
    echo "LANG=${locale}" > ${mountpoint}/etc/locale.conf
    echo "LC_MESSAGES=${locale}" >> ${mountpoint}/etc/locale.conf

    msg "enable glibc locales from locale.conf"
    for i in $(grep "^LANG" ${mountpoint}/etc/locale.conf | sed -e 's/.*=//g' -e's/\..*//g'); do
        sed -i -e "s/^#$i/$i/g" ${mountpoint}/etc/locale.gen
    done
    # always enable en_US
    sed -i 's/^#\(en_US.*\)/\1/' ${mountpoint}/etc/locale.gen

    msg "regenerate the locales"
    chroot ${mountpoint} /usr/bin/locale-gen

    msg_job_done "job_regenerate_locales"
}
