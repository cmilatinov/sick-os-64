// Global Descriptor Table
#include "arch/x84_64/gdt.h"

// Memory Management Unit
#include "arch/x84_64/mmu.h"

// Kernel Header
#include "arch/x84_64/kernel.h"

// Model Specific Register
#include "arch/x84_64/msr.h"

// Extra Definitions
#define TWO_MEGABYTES_SHIFT 21
#define TWO_MEGABYTES (1 << TWO_MEGABYTES_SHIFT)
#define KERNEL_BOOT_STACK_SIZE 0x4000
#define KERNEL_BOOT_STACK_ALIGNMENT 0x1000

// Multiboot Variables
.set MAGIC, 0x1badb002
.set FLAGS, (1 << 0 | 1 << 1)
.set CHECKSUM, -(MAGIC + FLAGS)

// Multiboot Header
.section .multiboot
	.long MAGIC
	.long FLAGS
	.long CHECKSUM

.code32

.section .bss

// Page Map Level 4 Table
.comm pml4, PML4_SIZE, PML4_ALIGNMENT

// High and Low Page Directory Pointer Tables
.comm low_pdpt, PDPT_SIZE, PDPT_ALIGNMENT
.comm high_pdpt, PDPT_SIZE, PDPT_ALIGNMENT

// High and Low Page Directory Tables
.comm low_page_directory_table, PAGE_DIRECTORY_SIZE, PAGE_DIRECTORY_ALIGNMENT
.comm high_page_directory_table, PAGE_DIRECTORY_SIZE, PAGE_DIRECTORY_ALIGNMENT

// Temporary Stack
.comm tmp_stack, KERNEL_BOOT_STACK_SIZE, KERNEL_BOOT_STACK_ALIGNMENT

.data

// Align GDT properly in memory.
.align GDT_ALIGNMENT

gdt_start:

    .quad GDT_FIRST_ENTRY      // Unused / Invalid Entry
    .quad GDT_KERNEL_ENTRY     // Kernel Entry

gdt_end:

    // Empty space for more entries.
    .skip (GDT_SIZE - (gdt_end - gdt_start))

gdt_ptr:

    // LGDT instruction requires a pointer to 2 Byte GDT Size + 4 Byte GDT pointer.
    .short GDT_SIZE - 1
    .long gdt_start

.section .text

// Setup entry point as function.
.global _start

_start:

    // Set Stack Pointer to point to the end of the stack as the stack grows downwards.
    movl $tmp_stack + KERNEL_BOOT_STACK_SIZE, %esp

    // PML4 entry for Low PDPT
    movl $low_pdpt, %eax                                                                        // Pointer to Low PDPT
    or $(MMU_PRESENT | MMU_WRITABLE), %eax                                                      // Set Page Present and Writable Flags
    movl %eax, pml4 + (PML4_ADDR_TO_ENTRY_INDEX(KERNEL_PHYSICAL_START) * PML4_ENTRY_SIZE)       // Copy Entry

    // PML4 entry for High PDPT
    movl $high_pdpt, %eax                                                                       // Pointer to High PDPT
    or $(MMU_PRESENT | MMU_WRITABLE), %eax                                                      // Set Page Present and Writable Flags
    movl %eax, pml4 + (PML4_ADDR_TO_ENTRY_INDEX(KERNEL_VIRTUAL_START) * PML4_ENTRY_SIZE)        // Copy Entry

    // PDPT entry for Low PDT
    movl $low_page_directory_table, %eax                                                        // Pointer to Low PDT
    or $(MMU_PRESENT | MMU_WRITABLE), %eax                                                      // Set Page Present and Writable Flags
    movl %eax, low_pdpt + (PDPT_ADDR_TO_ENTRY_INDEX(KERNEL_PHYSICAL_START) * PDPT_ENTRY_SIZE)   // Copy Entry

    // PDPT entry for High PDT
    movl $high_page_directory_table, %eax                                                       // Pointer to High PDT
    or $(MMU_PRESENT | MMU_WRITABLE), %eax                                                      // Set Page Present and Writable Flags
    movl %eax, high_pdpt + (PDPT_ADDR_TO_ENTRY_INDEX(KERNEL_VIRTUAL_START) * PDPT_ENTRY_SIZE)   // Copy Entry

    // Clear ECX Register for later use.
    xor %ecx, %ecx
    
    // Load physical kernel end address and divide it by 2MB (the page size).
    movl $_kernel_physical_end, %esi
    shrl $TWO_MEGABYTES_SHIFT, %esi
    addl $1, %esi

page_directory_table_loop:

    // EAX = 2MB * ECX
    movl $TWO_MEGABYTES, %eax
    mul %ecx

    // Set present, writable, 2MB page size flags.
    or $(MMU_PRESENT | MMU_WRITABLE | MMU_PDE_TWO_MB), %eax

    // Create an entry in High and Low PDT.
    movl %eax, low_page_directory_table(, %ecx, PAGE_DIRECTORY_ENTRY_SIZE)
    movl %eax, high_page_directory_table(, %ecx, PAGE_DIRECTORY_ENTRY_SIZE)

    // Increment counter.
    inc %ecx

    // Loop again if we havent reached the physical kernel end address.
    cmp %esi, %ecx
    jne page_directory_table_loop 

    // Load PML4 address in Control Register 3
    movl $pml4, %eax
    movl %eax, %cr3

    // Load Control Register 4 flags.
    movl $KERNEL_CR4, %eax
    movl %eax, %cr4

    // Read EFER MSR
    movl $MSR_EFER, %ecx
    rdmsr

    // Enable Long Mode
    or $MSR_EFER_LME, %eax
    wrmsr

    // Load Control Register 0 flags.
    movl $KERNEL_CR0, %eax
    movl %eax, %cr0

    // Load Global Descriptor Table
    lgdt gdt_ptr

    // Long Jump to 64-bit code
    ljmp $(KERNEL_GDT_ENTRY * GDT_ENTRY_SIZE), $_start64

.code64

// Setup 64-bit entry point as function.
.global _start64

_start64:

    // Set segment selectors to 0.
    xor %ax, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss

    // Pass the GDT pointer and size to C++ entry point.
    movq $gdt_start, %rdi
    movq $GDT_SIZE, %rsi

    // Call C++ entry point.
    call kernelMain

end_loop:

    // Clear Interrupts, Halt, and Loop
    cli
    hlt
    jmp end_loop