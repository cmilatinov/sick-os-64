#include "utils/lib.h"

#include "hardware/gdt.h"
#include "hardware/interrupts.h"
#include "drivers/keyboard.h"
#include "drivers/mouse.h"
#include "utils/keys.h"

void onKeyDown(char c, uint8_t keycode, uint8_t mods) {
	if(!(mods & MOD_CTRL || mods & MOD_ALT))
		printc(c);
}

void onMouseDown(uint8_t button){
	printf(" DOWN ");
	printh(button);
}

void onMouseUp(uint8_t button){
	printf(" UP ");
	printh(button);
}

void onMouseMove(uint32_t dx, uint32_t dy) {
	printf(" MOVE ( ");
	printh(dx);
	printf(" ");
	printh(~dy + 1);
	printf(" )");
}

extern "C" void kernelMain(void * gdtPtr, uint64_t gdtSize){

	clear();

	GDT _gdt(gdtPtr, KERNEL_GDT_ENTRY, gdtSize);
	GDT * gdt = &_gdt;

	InterruptManager iManager(IRQ_OFFSET_X86_64);

	KeyboardDriver kb;
	MouseDriver mb;
	kb.OnKeyDown(onKeyDown);
	mb.OnMouseButtonDown(onMouseDown);
	mb.OnMouseButtonUp(onMouseUp);
	mb.OnMouseMove(onMouseMove);

	iManager.SetInterruptHandler(0x21, &kb);
	iManager.SetInterruptHandler(0x2C, &mb);

	kb.Activate();
	mb.Activate();
	
	iManager.Activate();


	printf("=========================== This is a 64-bit SickOS! ===========================");
	printf();

	uint64_t value;
	asm volatile("movq %%rbp, %0" : "=r"(value));

	printf("GDT Size - ");
	printh(gdtSize);
	printf("\n\n");

	printf("GDT Pointer - ");
	printp(gdtPtr);
	printf("\n\n");

	uint8_t reg[10];
	uint16_t * ptr = (uint16_t*)reg;
	asm volatile("sgdt %0" : "=rm"(reg));

	printf("GDT Size - ");
	printh(ptr[0]);
	printf("\n\n");

	printf("GDT Pointer - ");
	printh(*reinterpret_cast<uint64_t*>(&reinterpret_cast<uint16_t*>(reg)[1]));
	printf("\n\n");

	printf("RBP - ");
	printh(value);
	printf("\n\n");

	asm volatile("movq %%rsp, %0" : "=r"(value));

	printf("RSP - ");
	printh(value);
	printf("\n\n");

	asm volatile(
		"lea 0(%%rip), %%rax"
		: "=a"(value));

	printf("RIP - ");
	printh(value);
	printf("\n\n");

	while(1);

}