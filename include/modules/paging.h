#include "common/types.h"

// Page table flags, 9 total flags
#define PAGE_FLAG_GLOBAL             (1 << 8)
#define PAGE_FLAG_SIZE               (1 << 7)
#define PAGE_FLAG_DIRTY              (1 << 6)
#define PAGE_FLAG_ACCESSED           (1 << 5)
#define PAGE_FLAG_CACHE_DISABLED     (1 << 4)
#define PAGE_FLAG_WRITE_THROUGH      (1 << 3)
#define PAGE_FLAG_USER               (1 << 2)
#define PAGE_FLAG_WRITABLE           (1 << 1)
#define PAGE_FLAG_PRESENT            (1 << 0)

// Number of entries in a page table
#define PT_NUM_ENTRIES          512

// Size of a page table (512 entries, 8 bytes per entry, 4 KB total)
#define PT_SIZE                 0x1000

// Number of bits indicating a page table index 
// 0 - 511, so 9 bits
#define PT_INDEX_SIZE           9

// Page table index mask, 9 ones in least significant bits, 0x1FF
#define PT_INDEX_MASK           0x1FF

// Page size (2 MB or 4 KB), 2 MB because PSE is enabled
#define PAGE_SIZE               0x200000

// Number of bits needed for a page offset (21 or 12), 21 for 2 MB pages
#define PAGE_OFFSET_SIZE        21

// Paging levels (3 or 4), 3 for 2 MB pages
#define PAGING_LEVELS           3

// Convert a virtual address to correct table index for given page level (0 - 3)
#define ADDR_TO_TABLE_INDEX(addr, level) (addr >> (PAGE_OFFSET_SIZE + PT_INDEX_SIZE * level)) & PT_INDEX_MASK

// Align a memory address down to byte boundary multiple of 2
#define ALIGN_DOWN(addr, align) (addr & ~(align - 1))

// Align a memory address up to byte boundary multiple of 2
#define ALIGN_UP(addr, align) ((addr + align - 1) & ~(align - 1))

// Retrieve alignment offset of an address
#define ALIGN_OFFSET(addr, align) (addr & (align - 1))

struct PageTable {

    struct Entry {

        uint64_t entry;
        
        Entry(void * address, uint16_t flags);

        inline uint16_t Flags() {
            return entry & 0x1FF;
        }
        inline PageTable * TableAddress() {
            return reinterpret_cast<PageTable*>(entry & 0xFFFFFFFFFF000); 
        }
        inline void * PageAddress() {
            return reinterpret_cast<void*>(entry & 0xFFFFFFFFFF000); 
        }

    } PACKED;

    Entry entries[PT_NUM_ENTRIES];

} PACKED;


namespace Paging {

    void Init(PageTable * pagingSpace, uint32_t size);

    bool Map(uint64_t physical, uint64_t address, uint32_t numPages);

};