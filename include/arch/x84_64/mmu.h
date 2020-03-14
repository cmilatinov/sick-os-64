// Memory Management Unit values.
#pragma once

#include "kernel.h"

// Page Map Level 4
#define PML4_SIZE 0x1000
#define PML4_ALIGNMENT 0x1000
#define PML4_ENTRY_SIZE 8
#define PML4_ADDR_TO_ENTRY_INDEX(addr) (((addr) >> 39) & 0x1FF)

// Page Directory Pointer Table
#define PDPT_SIZE 0x1000
#define PDPT_ALIGNMENT 0x1000
#define PDPT_ENTRY_SIZE 8
#define PDPT_ADDR_TO_ENTRY_INDEX(addr) (((addr) >> 30) & 0x1FF)

// Page Directory Table
#define PAGE_DIRECTORY_SIZE 0x1000
#define PAGE_DIRECTORY_ALIGNMENT 0x1000
#define PAGE_DIRECTORY_ENTRY_SIZE 8

// Page Table
#define PAGE_TABLE_SIZE 0x1000
#define PAGE_TABLE_ALIGNMENT 0x1000
#define PAGE_TABLE_ENTRY_SIZE 8

// MMU Flags.
#define MMU_PRESENT (1 << 0)
#define MMU_WRITABLE (1 << 1)
#define MMU_USER_MEMORY (1 << 2)
#define MMU_PDE_TWO_MB (1 << 7)
