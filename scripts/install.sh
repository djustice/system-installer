#!/bin/bash
#
#**************************************************************************
#   Copyright (C) 2007-2008 Jan Mette                                     *
#                      2012 Manuel Tortosa                                *
#                           <manutortosa@chakra-project.org>              *
#                                                                         *
#   uses some small fragments of the arch linux installer:                *
#   http://projects.archlinux.org/?p=installer.git;a=tree                 *
#                                                                         *
#   Much of this stuff can be implemented in a better/cleaner or smaller  *
#   fashion. I am not a bash pro anyway :) If you have some ideas for     *
#   improvements or created a patch (preferred, of course), just          *
#   contact me or use our bugtracker. Thanks.                             *
#                                                                         *
#   This script is free software; you can redistribute it and/or modify   *
#   it under the terms of the GNU General Public License as published by  *
#   the Free Software Foundation; either version 2 of the License, or     *
#   (at your option) any later version.                                   *
#                                                                         *
#   This script is distributed in the hope that it will be useful,        *
#   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
#   GNU General Public License for more details.                          *
#                                                                         *
#   You should have received a copy of the GNU General Public License     *
#   along with this program; if not, write to the                         *
#   Free Software Foundation, Inc.,                                       *
#   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.          *
#**************************************************************************






###########################################################################
#
# STARTUP
#
###########################################################################

# we need to be root to start this
if [ $UID -ne 0 ]; then
        echo "Only root can run $0"; exit 1
fi

# source functions we need & check if we are running the live system
# (functions.d/cmdline) is needed to parse kernel command line parameters
if [ -d "/bootmnt" ] ; then
        kernel_cmdline ()
        {
                for param in $(/bin/cat /proc/cmdline); do
                        case "${param}" in
                                $1=*) echo "${param##*=}"; return 0 ;;
                                $1) return 0 ;;
                                *) continue ;;
                        esac
                done
                [ -n "${2}" ] && echo "${2}"
                return 1
        }
        # force to use english
        export LANG=en_US.UTF-8
        export LC_MESSAGES=en_US.UTF-8
        LOCALE="$(kernel_cmdline locale.LANG en_US.utf8)"
        TIMEZONE="$(kernel_cmdline timezone UTC)"
        KEYMAP="$(kernel_cmdline vconsole.keymap us)"
        CONSOLEFONT="$(kernel_cmdline vconsole.font)"
        CONSOLEMAP="$(kernel_cmdline vconsole.font.map)"
        USENONFREE="$(kernel_cmdline nonfree no)"
        COUNTRY="$(kernel_cmdline lang)"
else
        clear
        echo " "
        echo "You are not running the live system"
        echo " "
        echo "Something is WRONG"
        echo " - or -"
        echo "You are just running the script for"
        echo "debug purposes, keep calm :)"
        echo " "
        echo "Everything could happen from here ..."
        echo " "
        sleep 6
fi

# source the config
. @CMAKE_INSTALL_PREFIX@/share/tribe/config/postinstall.conf

# load helper functions
. @CMAKE_INSTALL_PREFIX@/share/tribe/scripts/helpers

# load jobs
for f in @CMAKE_INSTALL_PREFIX@/share/tribe/scripts/*-job-*; do
  [ -e $f ] && . $f
done

# load init
. @CMAKE_INSTALL_PREFIX@/share/tribe/scripts/init

# parse parameters
while [ $# -gt 0 ]
        do
                case $1
                in
                        --job)
                        job=$2
                        shift 2
                        ;;

                        --use-swap)
                        use_swap=$2
                        shift 2
                        ;;

                        --target-swap)
                        target_swap=$2
                        shift 2
                        ;;

                        --target-root)
                        target_root=$2
                        shift 2
                        ;;

                        --use-boot)
                        use_boot=$2
                        shift 2
                        ;;

                        --target-boot)
                        target_boot=$2
                        shift 2
                        ;;

                        --target-boot-fs)
                        target_boot_fs=$2
                        shift 2
                        ;;

                        --use-home)
                        use_home=$2
                        shift 2
                        ;;

                        --use-opt)
                        use_opt=$2
                        shift 2
                        ;;

                        --use-tmp)
                        use_tmp=$2
                        shift 2
                        ;;

                        --use-usr)
                        use_usr=$2
                        shift 2
                        ;;

                        --use-var)
                        use_var=$2
                        shift 2
                        ;;

                        --extra-mountpoint)
                        extra_mountpoint=$2
                        shift 2
                        ;;

                        --extra-mountpoint-target)
                        extra_mountpoint_target=$2
                        shift 2
                        ;;

                        --mountpoint)
                        mountpoint=$2
                        shift 2
                        ;;

                        --hostname)
                        hostname=$2
                        shift 2
                        ;;
                        
                        --kblayout)
                        kblayout=$2
                        shift 2
                        ;;
                        
                        --kbvariant)
                        kbvariant=$2
                        shift 2
                        ;;
                        
                        --timezone)
                        timezone=$2
                        shift 2
                        ;;

                        --locale)
                        locale=$2
                        shift 2
                        ;;

                        --kdelang)
                        kdelang=$2
                        shift 2
                        ;;

                        --configure-pacman)
                        config_pacman=$2
                        shift 2
                        ;;

                        --user-name)
                        user_name=$2
                        shift 2
                        ;;

                        --user-passwd)
                        user_passwd=$2
                        shift 2
                        ;;

                        --root-passwd)
                        root_passwd=$2
                        shift 2
                        ;;

                        --netinst)
                        netinst=$2
                        shift 2
                        ;;

                        *)
                        clear
                        echo " "
                        echo " > Postinstall 0.9.0 "
                        echo " "
                        echo " > Usage: "
                        echo " "
                        echo "   --job            : the job to that should be performed"
                        echo "                   EXAMPLE: --job basic-setup"
                        echo " "
                        echo "   --use-swap       : you must set this to yes or no"
                        echo "                   EXAMPLE: --use-swap yes"
                        echo "   --target-swap    : the name of the target swap partition (just sdaX, not /dev/sdaX)"
                        echo "                   EXAMPLE: --target-swap sda2"
                        echo " "
                        echo "   --target-root    : the name of the target root partition (just sdaX, not /dev/sdaX)"
                        echo "                   EXAMPLE: --target sda1"
                        echo " "
                        echo "   --use-boot       : you must set this to yes or no"
                        echo "                   EXAMPLE: --use-boot yes"
                        echo "   --target-boot    : the name of the target boot partition (just sdaX, not /dev/sdaX)"
                        echo "                   EXAMPLE: --target-boot sda1"
                        echo "   --target-boot-fs : the filesystem of the target boot partition"
                        echo "                   EXAMPLE: --target-boot-fs ext2"
                        echo " "
                        echo "   --use-home       : you must set this to yes or no"
                        echo "                   EXAMPLE: --use-home yes"
                        echo " "
                        echo "   --use-opt        : you must set this to yes or no"
                        echo "                   EXAMPLE: --use-opt yes"
                        echo " "
                        echo "   --use-tmp        : you must set this to yes or no"
                        echo "                   EXAMPLE: --use-tmp yes"
                        echo " "
                        echo "   --use-usr        : you must set this to yes or no"
                        echo "                   EXAMPLE: --use-usr yes"
                        echo " "
                        echo "   --use-var        : you must set this to yes or no"
                        echo "                   EXAMPLE: --use-var yes"
                        echo " "
                        echo "   --extra-mountpoint        : the mountpoint for any additional partition"
                        echo "                   EXAMPLE: --extra-mountpoint /home/bleh/media"
                        echo "   --extra-mountpoint-target     : the name of the target var  partition (just sdaX, not /dev/sdaX)"
                        echo "                   EXAMPLE: --extra-mountpoint-target sda1"
                        echo " "
                        echo "   --mountpoint     : defines the mountpoint of the install target"
                        echo "                   EXAMPLE: --mountpoint /mnt/target"
                        echo "   --hostname       : defines the hostname of the target system"
                        echo "                   EXAMPLE: --hostname foobar"
                        echo "   --user-name      : username, used when we create a user"
                        echo "                   EXAMPLE: --user-name foo"
                        echo "   --user-passwd    : user password, used when we create a user"
                        echo "                   EXAMPLE: --user-passwd bar"
                        echo "   --root-passwd    : root password "
                        echo "                      EXAMPLE: --root-passwd bar"
                        echo "   --netinst        :  you must set this to yes or no"
                        echo "                            --netinst yes"
                        echo " "
                        echo " > Available jobs "
                        echo " "
                        echo "   01. initialize-target (creates special directories and devices on the target)"
                        echo "   02. configure-users (setup sudoers and groups for the target user)"
                        echo "   03. configure-sudoers (setup the sudoers file)"
                        echo "   04. configure-pacman (tries to find the best mirror for pacman and runs pacman -Syy afterwards)"
                        echo "   05. pre-remove (removes all live-specific packages from the target)"
                        echo "   06. pre-install (install needed packages for installation into the target)"
                        echo "   07. setup-xorg (copy autogenerated xorg.conf from live system to target)"
                        echo "   08. download-l10n (downloads l10n packages, if we have network)"
                        echo "   09. create-fstab (create fstab for the installed system)"
                        echo "   10. add-extra-mountpoint (create a additional mount, this can be run more than once)"
                        echo "   11. setup-hardware (add detected modules to rc.conf and mkinitcpio.conf on the target)"
                        echo "   12. create-initrd (create initrd for the installed system)"
                        echo "   13. regenerate-locales (apply locales and regenerate them)"
                        echo "   14. create-localtime (symlink the correct timezone file to to /etc/localtime)"
                        echo "   15. set-keymap (change the keymap in /etc/vconsole.conf) and set the keyboard map"
                        echo "   16. set-hostname (set hostname in /etc/hostname and /etc/hosts)"
                        echo "   17. cleanup-l10n (cleanup drivers and stuff)"
                        echo "   18. cleanup-drivers (cleanup drivers and stuff)"
                        echo "   19. cleanup-etc (to manually remove unneeded files etc)"
                        echo "   20. install-grub2 (install grub2 on the installed system)"
                        echo " "
                        shift 1
                        ;;
        esac
done






###########################################################################
#
# Now check if we have a job
#
###########################################################################
if [ -z "$job" ] ; then
                echo " "
                echo " > You need to specify some parameters, try --help "
                echo " "
		exit 1
fi






###########################################################################
#
# Ok, seems we have a job, lets start the init
#
###########################################################################
job_init

