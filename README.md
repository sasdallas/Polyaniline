# Polyaniline
In-progress bootloader for Ethereal, Currently supports UEFI.

## Description
Polyaniline is a bootloader for the Hexahedron kernel and Ethereal based off of GNUEFI. It has a menu system and (eventually) will be able to load other ELF kernels.\
This bootloader is developed on and off along with Ethereal.

## Screenshots

![Main menu](https://github.com/user-attachments/assets/6de6b158-9755-4f4c-bd3a-93ea2f4f36c7)\
*Polyaniline at the main menu*

![Config menu](https://github.com/user-attachments/assets/cf53b991-4253-47bf-b75f-4d94aa4be052)
*Polyaniline at the configure menu*

## Building

You must have GNU-EFI and a copy of Ethereal. Add your `hexahedron-kernel.elf` and `initrd.tar.img` to `emu-files` before building.

Simply run `make all` to build a BOOTx64.EFI image, FAT image, and CDROM image.\
**Warning:** The CDROM image has not been tested.

Run `make qemu_efi` (you might have to adjust the OVMF paths) to start an EFI session.

## License

Polyaniline, unlike Hexahedron/Ethereal (at the current moment), is licensed under a GPLv3 license. Any other files not marked as GPLv3 should not be treated as such.\
If you have found your code in this repo and wish for it to be removed, please contact me at sasdallas765@gmail.com
