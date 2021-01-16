#include "hardware/interrupts.h"
#include "hardware/cpu.h"
#include "hardware/pci.h"

#include "drivers/keyboard.h"
#include "drivers/mouse.h"
#include "drivers/svga/svga.h"

#include "modules/lib.h"
#include "modules/memory.h"
#include "modules/multiboot.h"
#include "modules/paging.h"

#include "common/keycodes.h"

#include "arch/x84_64/kernel.h"
#include "arch/x84_64/mmu.h"


void onKeyDown(char c, uint8_t key, uint8_t mods) {
	if (key == VK_BACKSPACE)
		return erase(1);
	printc(c);
}

extern "C" void kernelMain(void * multiboot, void * pagingSpace, void * heapStart){

	clear();

	MultibootLoader loader(multiboot);
	MultibootInfoMemory memInfo;
	loader.GetMemoryInfo(&memInfo);

	Paging::Init(reinterpret_cast<PageTable*>(pagingSpace), KERNEL_PAGING_SIZE / PT_SIZE);

	MemoryManager memManager(heapStart, KERNEL_HEAP_SIZE);

	InterruptManager iManager;
	iManager.Activate();

	DriverManager driverManager;
	KeyboardDriver keyboard;
	MouseDriver mouse;

	driverManager.RegisterDriver(&keyboard);
	driverManager.RegisterDriver(&mouse);
	PCI::ScanBus();

	keyboard.OnKeyDown(onKeyDown);

	driverManager.LoadAll();

	SVGA svga;
	svga.Init();
	
	while(1);

}