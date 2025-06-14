#!/usr/bin/bash

# build_efi_image.sh
# Builds an EFI image (CDROM/USB) for burning or use in QEMU


# OUTDIR is first argument
OUTDIR=$1


# Calculate required space
SPACE_REQ=$(du -sb "/home/samuel/Desktop/reduceOS/build-output/sysroot/boot/" | cut -f 1)
let "SIZE = ($SPACE_REQ / 940000)"
echo $SIZE

# For larger disk sizes use more SPC
if [ $SIZE -gt 128 ]; then
    SPC=4
else
    SPC=1
fi

# Create the FAT filesystem image
echo "-- Creating FAT filesystem (make sure you have mtools installed)..."
dd if=/dev/zero of=$OUTDIR/efi_fat.img bs=1M count=${SIZE}
# mformat -i $OUTDIR/efi_fat.img -f 2880 ::
mkfs.fat -s $SPC -S 2048 $OUTDIR/efi_fat.img 
mcopy -i $OUTDIR/efi_fat.img $OUTDIR/../../scripts/startup.nsh ::
mmd -i $OUTDIR/efi_fat.img ::/EFI
mmd -i $OUTDIR/efi_fat.img ::/EFI/BOOT 
mcopy -i $OUTDIR/efi_fat.img $OUTDIR/bootx64.efi ::/EFI/BOOT

mcopy -i $OUTDIR/efi_fat.img /home/samuel/Desktop/reduceOS/build-output/sysroot/boot/hexahedron-kernel.elf ::
mcopy -i $OUTDIR/efi_fat.img /home/samuel/Desktop/reduceOS/build-output/sysroot/boot/initrd.tar.img ::



echo "-- Creating CD image..."
mkdir $OUTDIR/isotemp
cp $OUTDIR/efi_fat.img $OUTDIR/isotemp/
xorriso -as mkisofs -R -f -e efi_fat.img -no-emul-boot -o $OUTDIR/efi_cdrom.iso $OUTDIR/isotemp
rm -r $OUTDIR/isotemp

echo "-- Done!"
