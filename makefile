BUILD = build
INCLUDE = include
SRC = src

CC = gcc

SHARED_FLAGS = -I$(INCLUDE) -nostdinc -nostdlib  -g -Wall -Wextra 					\
            -MMD -mno-red-zone -mcmodel=large -O1									\
			-ffreestanding -fno-pie -fno-permissive -fno-exceptions -fno-rtti		\
			-fno-builtin 															\
			-Wno-unused-parameter
CXXFLAGS = $(SHARED_FLAGS) -c -std=c++17
ASFLAGS = $(SHARED_FLAGS) -Wa,--divide -c
BINFLAGS = $(SHARED_FLAGS) -z max-page-size=0x1000 -no-pie -Wl,--build-id=none
CFLAGS = 	-I$(INCLUDE) -nostdinc -nostdlib  -g -Wall -Wextra 						\
            -MMD -mno-red-zone -mcmodel=large -O1									\
			-ffreestanding -fno-pie -fno-exceptions									\
			-fno-builtin 															\
			-Wno-unused-parameter -c

CRTI_OBJ=$(BUILD)/crti.o
CRTN_OBJ=$(BUILD)/crtn.o

_OBJS = boot/boot.o 				\
		kernel.o 					\
		common/virtual.o 			\
		hardware/ports.o 			\
		hardware/interrupts.o 		\
		hardware/interruptstubs.o 	\
		hardware/cpu.o				\
		hardware/pci.o				\
		modules/lib.o 				\
		modules/memory.o			\
		modules/multiboot.o			\
		modules/paging.o			\
		modules/drivers.o			\
		drivers/keyboard.o			\
		drivers/mouse.o				\
		drivers/svga/svga.o

LINK_OBJS = $(notdir $(_OBJS))
OBJS = $(patsubst %, $(BUILD)/%, $(_OBJS))

$(CRTI_OBJ) : $(SRC)/ctors/crti.c
	@echo ""
	@echo "Compiling CRTI source ($<)..."
	@echo ""
	@$(CC) $(CFLAGS) -o $@ $< 1> /dev/null

$(CRTN_OBJ) : $(SRC)/ctors/crtn.c
	@echo ""
	@echo "Compiling CRTN source ($<)..."
	@echo ""
	@$(CC) $(CFLAGS) -o $@ $< 1> /dev/null

$(BUILD)/%.o: $(SRC)/%.S
	@echo ""
	@echo "Compiling Assembly source ($<)..."
	@echo ""
	@$(CC) $(ASFLAGS) -o $(BUILD)/$(notdir $@) $< 1> /dev/null

$(BUILD)/%.o: $(SRC)/%.cpp
	@echo ""
	@echo "Compiling C++ source ($<)..."
	@echo ""
	@$(CC) $(CXXFLAGS) -o $(BUILD)/$(notdir $@) $< 1> /dev/null

kernel.bin: $(OBJS) $(CRTI_OBJ) $(CRTN_OBJ)
	@echo ""
	@echo "Compiling kernel..."
	@echo ""
	@cd build && $(CC) $(BINFLAGS) -L$(BUILD) -T ../$(SRC)/linker.ld -o ../$@ $(notdir $(CRTI_OBJ)) $(LINK_OBJS) $(notdir $(CRTN_OBJ))

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
	@echo "    multiboot2 /boot/$<" 			>> iso/boot/grub/grub.cfg
	@echo "    boot" 							>> iso/boot/grub/grub.cfg
	@echo } 									>> iso/boot/grub/grub.cfg
	@grub-mkrescue --output=$@ iso
	@rm -rf iso

.PHONY: run
run: kill clean kernel.iso
	@echo "" 
	@echo "Starting x86_64 emulator..."
	@echo ""
	@qemu-system-x86_64.exe -cdrom $(word 3, $^) -serial stdio -m 1024M -cpu max -vga vmware

.PHONY: kill
kill: 
	@echo "" 
	@echo "Killing running emulators..."
	@echo ""
	@taskkill.exe /f /im "qemu-system-x86_64.exe" > nul 2>&1 || exit 0
	@echo -n "  3   "
	@sleep 0.4
	@echo -n "  2   "
	@sleep 0.4
	@echo -n "  1   "
	@sleep 0.4
	@echo "  0"


.PHONY: clean
clean: 
	@echo ""
	@echo "Cleaning old build..."
	@echo ""
	@rm -rf build
	@rm -f kernel.iso
	@rm -f kernel.bin
	@mkdir build