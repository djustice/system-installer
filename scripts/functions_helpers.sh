#!/bin/bash

function error_handler {
echo " "
echo " "
echo " "
echo " "
echo "Something went wrong :( ..."
echo " "
echo " "
echo " "
echo " "
exit 1
}

api_fs_umount() {
    msg "unmount api filesystem"
    umount \
        "$1/tmp" \
        "$1/run" \
        "$1/dev/shm" \
        "$1/dev/pts" \
        "$1/dev" \
        "$1/sys" \
        "$1/proc"
}

api_fs_mount() {
    msg "mount api filesystem"
    mount -t proc proc "$1/proc" -o nosuid,noexec,nodev &&
        mount -t sysfs sys "$1/sys" -o nosuid,noexec,nodev &&
        mount -t devtmpfs udev "$1/dev" -o mode=0755,nosuid &&
        mount -t devpts devpts "$1/dev/pts" -o mode=0620,gid=5,nosuid,noexec &&
        mount -t tmpfs shm "$1/dev/shm" -o mode=1777,nosuid,nodev &&
        mount -t tmpfs run "$1/run" -o nosuid,nodev,mode=0755 &&
        mount -t tmpfs tmp "$1/tmp" -o mode=1777,strictatime,nodev,nosuid
}

# standard message
msg() {
    local mesg=$1; shift
    echo ":: BACKEND : ${mesg}"
}

# message when starting a job
msg_job_start() {
    local mesg=$1; shift
    echo " "
    echo " "
    echo ">> STARTING JOB ${mesg}"
}

# message when a job has been finished
msg_job_done() {
    local mesg=$1; shift
    echo ">> JOB ${mesg} DONE"
    echo " "
    echo " "
}

# check if we have a internet connection (pinging a Level3 nameserver for reliability)
ping_check=$(ping -c 1 -W 1 4.2.2.1)

