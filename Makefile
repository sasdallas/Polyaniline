# =========== POLYANILINE MAKEFILE ===========

# ======= COMPILERS =========

EFI_CC = gcc
EFI_AS = as
EFI_LD = ld
EFI_OBJCOPY = objcopy

BIOS_CC = i686-elf-gcc
BIOS_LD = i686-elf-ld
BIOS_AS = i686-elf-as
BIOS_OBJCOPY = i686-elf-objcopy
BIOS_NASM = nasm -f bin

# ======= OUTPUT DIRECTORIES =======
OUTPUT_DIRECTORY = build-output
OUTPUT_EFI = $(OUTPUT_DIRECTORY)/efi
OUTPUT_BIOS = $(OUTPUT_DIRECTORY)/bios
OUTPUT_FINAL = $(OUTPUT_DIRECTORY)/final

# ======= EFI PLATFORM ========

# Edit this if you have a different GNUEFI include/lib directory
GNUEFI_INCLUDE_DIR = /usr/include/efi
GNUEFI_LIB_DIR = /usr/lib/
GNUEFI_LIB_DIR32 = /usr/lib32/

GNUEFI_EFI64_CRT0 = $(GNUEFI_LIB_DIR)/crt0-efi-x86_64.o
GNUEFI_EFI32_CRT0 = $(GNUEFI_LIB_DIR32)/crt0-efi-ia32.o

EFI_CFLAGS = -c -fno-stack-protector -ffreestanding -g -fpic -fshort-wchar -mno-red-zone -I$(GNUEFI_INCLUDE_DIR) -I$(GNUEFI_INCLUDE_DIR)/x86_64/ -Iinclude -DEFI_FUNCTION_WRAPPER  -Wall  -D__EFI__ -Wno-unused-but-set-variable -Wno-unused-label
EFI_LDFLAGS = -nostdlib -znocombreloc -T /usr/lib/elf_x86_64_efi.lds -shared -Bsymbolic -L /usr/lib/
EFI_OBJCOPYFLAGS = -j .text -j .sdata -j .data -j .rodata -j .dynamic -j .dynsym  -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc --target efi-app-x86_64 --subsystem=10

$(info $(EFI_LDFLAGS))

$(info $(EFI_CFLAGS))

# Collect EFI source objects
EFI_DIRECTORIES = platform/efi/ polyaniline polyaniline/loader minilib
EFI_OUTPUT_DIRECTORIES = $(addprefix $(OUTPUT_EFI)/, $(EFI_DIRECTORIES))
EFI_SOURCES = $(shell find  $(EFI_DIRECTORIES) -maxdepth 1 -name "*.c")
EFI_SOURCES_ASM = $(shell find $(EFI_DIRECTORIES) -maxdepth 1 -name "*.S")
EFI_OBJECTS = $(patsubst %.c, $(OUTPUT_EFI)/%.o, $(EFI_SOURCES)) $(patsubst %.S, $(OUTPUT_EFI)/%.o, $(EFI_SOURCES_ASM)) $(GENERIC_OBJECTS_EFI)



# ======= TARGETS (EFI) =======

# Make build directories
EFI_MAKE_BUILD_DIRECTORIES:
	-mkdir -pv $(OUTPUT_DIRECTORY)
	-mkdir -pv $(OUTPUT_EFI)
	-mkdir -pv $(OUTPUT_FINAL)
	-mkdir -pv $(EFI_OUTPUT_DIRECTORIES)

# Compiler of each C file
$(OUTPUT_EFI)/%.o: %.c Makefile
	$(EFI_CC) $< $(EFI_CFLAGS)  -o $@

# Compiler of each .S file
$(OUTPUT_EFI)/%.o: %.S Makefile
	$(EFI_CC) $< $(EFI_CFLAGS)  -o $@


# Main compiler, links everything together
$(OUTPUT_FINAL)/bootx64.efi: $(EFI_OBJECTS)
	$(EFI_LD) $(EFI_LDFLAGS) $(GNUEFI_EFI64_CRT0) $(EFI_OBJECTS) -o $(OUTPUT_EFI)/efi.so  -lgnuefi -lefi
	$(EFI_OBJCOPY) --only-keep-debug $(OUTPUT_EFI)/efi.so $(OUTPUT_FINAL)/bootx64.debug
	$(EFI_OBJCOPY) $(EFI_OBJCOPYFLAGS) $(OUTPUT_EFI)/efi.so $(OUTPUT_FINAL)/bootx64.efi


# Image builder
$(OUTPUT_FINAL)/efi_fat.img: $(OUTPUT_FINAL)/bootx64.efi
	bash -c "scripts/build_efi_image.sh $(shell pwd)/$(OUTPUT_FINAL)"

# ======= TARGETS (BIOS) =======

# ======= TARGETS (MAIN) =======

# Delete config.o
delete_config_object:
	-rm $(OUTPUT_EFI)/polyaniline/config.*
	-rm $(OUTPUT_BIOS)/polyaniline/config.*

# Build BIOS
bios:
	@echo "soon"

# Build EFI
efi: EFI_MAKE_BUILD_DIRECTORIES $(OUTPUT_FINAL)/efi_fat.img
	@printf "\n[ EFI TARGET BUILT SUCCESSFULLY ]\n"


# Build EFI and BIOS
all: efi bios

# Launch QEMU with EFI
qemu_efi:
	qemu-system-x86_64 -cpu qemu64 \
		-drive if=pflash,format=raw,unit=0,file=/usr/share/OVMF/x64/OVMF_CODE.4m.fd,readonly=on \
		-drive if=pflash,format=raw,unit=1,file=/usr/share/OVMF/x64/OVMF_VARS.4m.fd \
		-net none \
		-drive file=$(OUTPUT_FINAL)/efi_fat.img -s \
		-no-reboot -no-shutdown -d int

uefi_run:
	uefi-run $(OUTPUT_FINAL)/bootx64.efi

# Clean target
clean:
	-rm -rf $(OUTPUT_DIRECTORY)
