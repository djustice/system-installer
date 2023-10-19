#!/bin/bash

job_configure_sudoers()
{
    msg_job_start "job_configure_sudoers"

    # add given user to sudoers
    msg "adding the user to sudoers"
    echo >>${mountpoint}/etc/sudoers
    echo "# user created during chakra setup" >>${mountpoint}/etc/sudoers
    echo "${user_name}     ALL=(ALL) ALL" >>${mountpoint}/etc/sudoers
    echo >>${mountpoint}/etc/sudoers

    # ensure correct permissions
    msg "ensure correct sudoers permissions"
    chmod -v 0440 ${mountpoint}/etc/sudoers

    msg_job_done "job_configure_sudoers"
}
