#!/bin/bash

job_set_hostname()
{
    # initialize error handling
    trap error_handler ERR

    msg_job_start "job_set_hostname"

    msg "set the hostname"
    if [ "${hostname}" != "" ] ; then
        msg "creating /etc/hostname with '${hostname}'"
        echo "${hostname}" > ${mountpoint}/etc/hostname
        msg "updating /etc/hosts"
        sed -i -e 's/^[ \t]*127.0.0.1.*/127.0.0.1\tlocalhost.localdomain\tlocalhost\t'${hostname}'/' ${mountpoint}/etc/hosts
    else
        msg "creating /etc/hostname with 'chakra' (no hostname specified)"
        echo "chakra" > ${mountpoint}/etc/hostname
        msg "updating /etc/hosts"
        sed -i -e 's/^[ \t]*127.0.0.1.*/127.0.0.1\tlocalhost.localdomain\tlocalhost\tchakra/' ${mountpoint}/etc/hosts
    fi

    msg_job_done "job_set_hostname"
} 
