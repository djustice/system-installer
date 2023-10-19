#!/bin/bash

job_init()
{
    ###########################################################################
    # GLOBAL JOB DEPENDENCIES
    ###########################################################################
    if [ -z "${target_root}" ] ; then
        msg "ERROR: You need to give a partition for root with --target-root"
        msg "Stopping."
        exit 0
    fi

    if [ -z "${mountpoint}" ] ; then
        msg "ERROR: You need to give a mountpoint for the root partition with --mountpoint"
        msg "Stopping."
        echo " "
        exit 0
    fi

    ###########################################################################
    # TARGET SPECIFIC JOB DEPENDENCIES
    ###########################################################################
    if [ -z "${use_boot}" ] ; then
        use_boot=no
        msg "not using separate boot partition"
    fi
    if [ -z "${use_home}" ] ; then
        use_home=no
        msg "not using separate home partition"
    fi
    if [ -z "${use_opt}" ] ; then
        use_opt=no
        msg "not using separate opt partition"
    fi
    if [ -z "${use_tmp}" ] ; then
        use_tmp=no
        msg "not using separate tmp partition"
    fi
    if [ -z "${use_usr}" ] ; then
        use_usr=no
        msg "not using separate usr partition"
    fi
    if [ -z "${use_var}" ] ; then
        use_var=no
        msg "not using separate var partition"
    fi

    if [ "$use_boot" = "yes" ] ; then
        msg "using separate boot partition"
        if [ -z "${target_boot}" ] ; then
            msg "ERROR: You need to give me a partition for boot with --target-boot"        
            msg "Stopping."
            echo " "
            exit 0
        fi
        if [ -z "${target_boot_fs}" ] ; then
            msg "ERROR: You need to give me a fs for boot with --target-boot-fs"    
            msg "Stopping."
            echo " "
            exit 0
        fi
    fi

    if [ "$use_home" = "yes" ] ; then
        msg "using separate home partition"
    fi

    if [ "$use_opt" = "yes" ] ; then
        msg "using separate opt partition"
    fi

    if [ "$use_tmp" = "yes" ] ; then
        msg "using separate tmp partition"
    fi

    if [ "$use_usr" = "yes" ] ; then
        msg "using separate usr partition"
    fi

    if [ "$use_var" = "yes" ] ; then
        msg "using separate var partition"
    fi

    ###########################################################################
    # JOB SPECIFIC DEPENDENCIES
    ###########################################################################
    if [ "$job" = "setup-sudoers" ] ; then
        if [ -z "${mountpoint}" ] ; then
            msg "ERROR: You need to give a mountpoint for the root partition with --mountpoint "
            msg "Stopping. "
            echo " "
            exit 0
        fi
        if [ -z "${user_name}" ] ; then
            msg "ERROR: You need to give a username for this job with  --user-name "     
            msg "Stopping. "
            echo " "
            exit 0
        fi
    fi

    if [ "$job" = "basic-setup" ] ; then
        if [ -z "${mountpoint}" ] ; then
            msg "ERROR: You need to give a mountpoint for the root partition with --mountpoint "
            msg "Stopping. "
            echo " "
            exit 0
        fi
    fi

    if [ "$job" = "remove-livepkgs" ] ; then
        if [ -z "${mountpoint}" ] ; then
            msg "ERROR: You need to give a mountpoint for the root partition with --mountpoint "
            msg "Stopping. "
            echo " "
            exit 0
        fi
    fi

    if [ "$job" = "install-pkgs" ] ; then
        if [ -z "${mountpoint}" ] ; then
            msg "ERROR: You need to give a mountpoint for the root partition with --mountpoint "
            msg "Stopping. "
            echo " "
            exit 0
        fi
    fi

    if [ "$job" = "configure-hardware" ] ; then
        if [ -z "${mountpoint}" ] ; then
            msg "ERROR: You need to give a mountpoint for the root partition with --mountpoint "
            msg "Stopping. "
            echo " "
            exit 0
        fi
    fi

    if [ "$job" = "copy-xorgconf" ] ; then
        if [ -z "${mountpoint}" ] ; then
            msg "ERROR: You need to give a mountpoint for the root partition with --mountpoint "
            msg "Stopping. "
            echo " "
            exit 0
        fi
    fi

    if [ "$job" = "create-fstab" ] ; then

        if [ -z "${target_root}" ] ; then
            msg "ERROR: You need to give a partition for root with --target-root "
            msg "Stopping. "
            echo " "
            exit 0
        fi


        if [ -z "${mountpoint}" ] ; then
            msg "ERROR: You need to give a mountpoint for the root partition with --mountpoint "
            msg "Stopping. "
            echo " "
            exit 0
        fi

        if [ "$use_boot" = "yes" ] ; then
            if [ -z "${target_boot}" ] ; then
                msg "ERROR: You need to give a partition for boot with --target-boot "       
                msg "Stopping. "
                echo " "
                exit 0
            fi
            if [ -z "${target_boot_fs}" ] ; then
                msg "ERROR: You need to give a fs for boot with --target-boot-fs "                          
                msg "Stopping. "
                echo " "
                exit 0
            fi
        fi
    fi

    if [ "$job" = "create-initrd" ] ; then
        if [ -z "${mountpoint}" ] ; then
            msg "ERROR: You need to give a mountpoint for the root partition with --mountpoint "
            msg "Stopping. "
            echo " "
            exit 0
        fi
    fi

    if [ "$job" = "install-grub2" ] ; then
        if [ -z "${mountpoint}" ] ; then
            msg "ERROR: You need to give a mountpoint for the root partition with --mountpoint "
            msg "Stopping. "
            echo " "
            exit 0
        fi
        if [ -z "${target_boot}" ] ; then
            msg "ERROR: You need to give a mountpoint for the boot partition with --target-boot "
            msg "Stopping. "
            echo " "
            exit 0
        fi
    fi

    ###########################################################################
    # JOB HANDLING
    ###########################################################################
    case "$job" in
        initialize-target)
            job_initialize_target
            ;;
        configure-pacman)
            job_configure_pacman
            ;;
        pre-remove)
            job_pre_remove
            ;;
        pre-install)
            job_pre_install
            ;;
        setup-xorg)
            job_setup_xorg
            ;;
        setup-hardware)
            job_setup_hardware
            ;;
        download-l10n)
            job_download_l10n
            ;;
        create-localtime)
            job_create_localtime
            ;;
        set-hostname)
            job_set_hostname
            ;;
        set-keymap)
            job_set_keymap
            ;;
        create-fstab)
            job_create_fstab
            ;;
        add-extra-mountpoint)
            job_add_extra_mountpoint
            ;;
        create-initrd)
            job_create_initrd
            ;;
        regenerate-locales)
            job_regenerate_locales
            ;;
        install-grub2)
            job_install_grub2
            ;;
        configure-sudoers)
            job_configure_sudoers
            ;;
        configure-users)
            job_configure_users
            ;;
        cleanup-l10n)
            job_cleanup_l10n
            ;;
        cleanup-drivers)
            job_cleanup_drivers
            ;;
        cleanup-etc)
            job_cleanup_etc
            ;;
        *)
            echo " > OOOPs... INVALID JOB"
            echo " "
            echo "   You know when you are falling asleep,"
            echo "   and you imagine yourself walking or"
            echo "   something, and suddenly you misstep,"
            echo "   stumble and jolt awake ?"
            echo " "
            echo "   Well, thats what an INVALID JOB feels"
            echo "   like. Double check your commands ok?"
            echo " "
            echo "   :("
            echo " "
            exit 1
            ;;
    esac
}
