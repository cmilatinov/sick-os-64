#include "lib.h"

#include "gdt.h"
#include "interrupts.h"
#include "keyboard.h"

extern "C" void kernelMain(void * gdtPtr, uint64_t gdtSize){

	clear();

	GDT _gdt(gdtPtr, KERNEL_GDT_ENTRY, gdtSize);
	GDT * gdt = &_gdt;

	InterruptManager iManager(0x20);
	KeyboardDriver kb(NULL);
	
	iManager.SetInterruptHandler(0x21, &kb);
	iManager.Activate();
	kb.Activate();
	
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


}