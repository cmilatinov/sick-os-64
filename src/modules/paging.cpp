#include "modules/paging.h"
#include "modules/lib.h"

namespace {
    
    // Pointer to the upmost paging level table (PML4)
    PageTable * pml4 = nullptr;

    // Paging space used to put more page tables
    PageTable * pagingSpace = nullptr;

    // Size of the paging space in number of tables
    uint32_t size = 0;

    // Amount of tables created in the paging space (zero at first)
    // Tables are created as needed in increasing index order
    uint32_t created = 0;

}

PageTable::Entry::Entry(void * address, uint16_t flags) {

    // Unset first 12 bits and last 16 bits of 
    entry = reinterpret_cast<uint64_t>(address) & 0xFFFFFFFFFF000;

    // First 12 bits are reserved for page flags / our OS's use.
    entry |= flags & 0xFFF;
}

void Paging::Init(PageTable * pagingSpace, uint32_t size) {
    uint64_t pml4Address;
    asm volatile("movq %%cr3, %0" : "=r"(pml4Address));
    ::pml4 = reinterpret_cast<PageTable*>(pml4Address);
    ::pagingSpace = pagingSpace;
    ::size = size;
}

bool Paging::Map(uint64_t physical, uint64_t address, uint32_t numPages) {

    // Pager not initialized / can't map zero pages.
    if (pml4 == nullptr || !numPages)
        return false;

    // Loop for each page to map.
    for (uint32_t p = 0; p < numPages; p++) {

        // Virtual page address
        uint64_t vAddr = address + (p * PAGE_SIZE);

        // Physical page address
        uint64_t pAddr = physical + (p * PAGE_SIZE);

        // Page table for the current level, start with PML4
        PageTable * current = pml4;

        // Loop for each paging level
        for (int8_t i = PAGING_LEVELS - 1; i >= 0; i--) {
            
            // Page table index composed of 9 bits
            uint16_t index = ADDR_TO_TABLE_INDEX(vAddr, i);

            // If the next paging entry is empty and we havent reached the last paging level.
            if (!current->entries[index].entry && i)

                // If we have paging space left, create a new page table and move to it.
                if (created < size) {
                    current->entries[index] = PageTable::Entry(pagingSpace + created++, PAGE_FLAG_PRESENT | PAGE_FLAG_WRITABLE);
                    current = current->entries[index].TableAddress();
                
                // Otherwise, memory mapping fails because a page has already been mapped at this virtual address.
                } else
                    return false;

            // Move to next paging table.
            else if (i)
                current = current->entries[index].TableAddress();

            // We've reached the last paging level.
            // Put the physical address of the page and proper flags in the correct entry.
            else
                current->entries[index] = PageTable::Entry(reinterpret_cast<void*>(pAddr), PAGE_FLAG_PRESENT | PAGE_FLAG_WRITABLE | (PAGING_LEVELS == 3 ? PAGE_FLAG_SIZE : 0));
            
        }
    }

    // Memory mapping succeeded!
    return true;
}