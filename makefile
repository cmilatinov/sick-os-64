BUILD = build
INCLUDE = include
SRC = src

CC = gcc
SHARED_FLAGS = -I$(INCLUDE) -nostdinc -nostdlib  -g -Wall -Wextra 					\
            -MMD -mno-red-zone -mcmodel=kernel 										\
			-ffreestanding -fno-pie -fno-permissive -fno-exceptions -fno-rtti		\
			-fno-builtin 															\
			-Wno-unused-parameter
CFLAGS = $(SHARED_FLAGS) -c
ASFLAGS = $(SHARED_FLAGS) -Wa,--divide -c
BINFLAGS = $(SHARED_FLAGS) -z max-page-size=0x1000 -no-pie -Wl,--build-id=none

_OBJS = boot/boot.o 				\
		kernel.o 					\
		common/virtual.o 			\
		modules/lib.o 				\
		modules/memory.o			\
		hardware/gdt.o 				\
		hardware/ports.o 			\
		hardware/interrupts.o 		\
		hardware/interruptstubs.o 	\
		drivers/keyboard.o			\
		drivers/mouse.o				\
		drivers/vga.o

LINK_OBJS = $(notdir $(_OBJS))
OBJS = $(patsubst %, $(BUILD)/%, $(_OBJS))

$(BUILD)/%.o: $(SRC)/%.S
	@echo ""
	@echo "Compiling Assembly source ($<)..."
	@echo ""
	@bash -c "$(CC) $(ASFLAGS) -o $(BUILD)/$(notdir $@) $< 1> /dev/null"

$(BUILD)/%.o: $(SRC)/%.cpp
	@echo ""
	@echo "Compiling C++ source ($<)..."
	@echo ""
	@bash -c "$(CC) $(CFLAGS) -o $(BUILD)/$(notdir $@) $< 1> /dev/null"

kernel.bin: $(OBJS)
	@echo ""
	@echo "Compiling kernel..."
	@echo ""
	@bash -c "cd build && $(CC) $(BINFLAGS) -L$(BUILD) -T ../$(SRC)/linker.ld -o ../$@ $(LINK_OBJS)"

kernel.iso: kernel.bin
	@echo ""
	@echo "Creating bootable ISO..."
	@echo ""
	@mkdir -p iso/boot/grub
	@cp $< iso/boot/
	@echo set timeout=0 						>> iso/boot/grub/grub.cfg
	@echo set default=0 						>> iso/boot/grub/grub.cfg
	@echo 										>> iso/boot/grub/grub.cfg
	@echo 'menuentry "SickOS" {' 				>> iso/boot/grub/grub.cfg
	@echo "    multiboot /boot/$<" 				>> iso/boot/grub/grub.cfg
	@echo "    boot" 							>> iso/boot/grub/grub.cfg
	@echo } 									>> iso/boot/grub/grub.cfg
	@bash -c "grub-mkrescue --output=$@ iso &> /dev/null"
	@rm -rf iso

.PHONY: run
run: clean kernel.iso
	@echo "" 
	@echo "Starting x86_64 emulator..."
	@echo ""
	@wcmd qemu-system-x86_64 -cdrom $(word 2, $^) -serial stdio -m 1024M || qemu-system-x86_64 -cdrom $(word 2, $^) -serial stdio -m 1024M

.PHONY: clean
clean: 
	@echo ""
	@echo "Cleaning old build..."
	@echo ""
	@rm -rf build
	@rm -f kernel.iso
	@rm -f kernel.bin
	@mkdir build