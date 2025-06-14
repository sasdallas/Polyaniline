# Polyaniline
In-progress bootloader for Ethereal, Currently supports UEFI.

## Description
Polyaniline is a bootloader for the Hexahedron kernel and Ethereal based off of GNUEFI. It has a menu system and (eventually) will be able to load other ELF kernels.\
This bootloader is developed on and off along with Ethereal.

## Screenshots

![Main menu](https://github.com/user-attachments/assets/deea5701-942e-43da-b6ff-2bb25e3ebadf)\
*Polyaniline at the main menu*

![Config menu](https://github.com/user-attachments/assets/7765da71-e5a2-424c-a080-7470b2554ecd)\
*Polyaniline at the configure menu*

## Building

You must have GNU-EFI and a copy of Ethereal. Add your `hexahedron-kernel.elf` and `initrd.tar.img` to `emu-files` before building.

Simply run `make all` to build a BOOTx64.EFI image, FAT image, and CDROM image.\
**Warning:** The CDROM image has not been tested.

Run `make qemu_efi` (you might have to adjust the OVMF paths) to start an EFI session.

## License

Polyaniline, unlike Hexahedron/Ethereal (at the current moment), is licensed under a GPLv3 license. Any other files not marked as GPLv3 should not be treated as such.\
If you have found your code in this repo and wish for it to be removed, please contact me at sasdallas765@gmail.com
