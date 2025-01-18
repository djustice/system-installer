# see Chakra scripts !!!

ls /usr/share/zoneinfo/${timezone_country}/${region} /etc/localtime

hwclock --systohc

edit /etc/locale.conf
locale-gen

set KEYMAP= in /etc/vconsole.conf
set /etc/hostname

passwd (set root pw)

create new ${user} account
passwd (set user pw)

grub-install --target=x86_64-efi --efi-directory=${boot_partition} --bootloader-id=GRUB
grub-mkconfig -o /boot/grub/grub.cfg
