#!/bin/bash

job_create_localtime()
{
    # initialize error handling
    trap error_handler ERR

    msg_job_start "job_create_localtime"

    #
    # This reads the $timezone variable and
    # creates a symlink from /usr/share/zoneinfo/ to /etc/localtime
    #

    timezone_real=`echo $timezone | sed 's/-/\//g'`
    sleep 0.5
    ln -sf /usr/share/zoneinfo/${timezone_real} ${mountpoint}/etc/localtime
    msg "Correct timezone: ${timezone_real}"
    unset timezone_real

    msg_job_done "job_create_localtime"
}
