// Global Descriptor Table entries and contants.
#pragma once

// Entry size.
#define GDT_ENTRY_SIZE 8

// Flags.
#define GDT_FLAG_FOUR_KILOBYTE_GRANULARITY (1 << 3)
#define GDT_FLAG_32BIT_PROTECTED_MODE (1 << 2)
#define GDT_FLAG_64BIT_MODE (1 << 1)

// Access rights.
#define GDT_ACCESS_PRESENT (1 << 7)
#define GDT_ACCESS_PRIVILEGE_RING0 (0x0 << 5)
#define GDT_ACCESS_PRIVILEGE_RING1 (0x1 << 5)
#define GDT_ACCESS_PRIVILEGE_RING2 (0x2 << 5)
#define GDT_ACCESS_PRIVILEGE_RING3 (0x3 << 5)
#define GDT_ACCESS_DESCRIPTOR_CODE (1 << 4)
#define GDT_ACCESS_DESCRIPTOR_DATA (1 << 4)
#define GDT_ACCESS_EXECUTABLE (1 << 3)
#define GDT_ACCESS_DIRECTION_DOWN (1 << 2)
#define GDT_ACCESS_READABLE_WRITABLE (1 << 1)

// Create GDT entry.
#define GDT_ENTRY(base, limit, flags, access)          \
  (                                                    \
    (((((base)) >> 24) & 0xFF) << 56) |                \
    ((((flags)) & 0xF) << 52) |                        \
    (((((limit)) >> 16) & 0xF) << 48) |                \
    (((((access) | (1 << 4))) & 0xFF) << 40) |         \
    ((((base)) & 0xFFF) << 16) |                       \
    (((limit)) & 0xFFFF)                               \
  )

// Unused entry.
#define GDT_FIRST_ENTRY 0

// Kernel entry.
#define GDT_KERNEL_ENTRY_CODE GDT_ENTRY(0, 0xffffffff, GDT_FLAG_FOUR_KILOBYTE_GRANULARITY | GDT_FLAG_64BIT_MODE, GDT_ACCESS_PRESENT | GDT_ACCESS_PRIVILEGE_RING0 | GDT_ACCESS_DESCRIPTOR_CODE | GDT_ACCESS_EXECUTABLE | GDT_ACCESS_READABLE_WRITABLE)
#define GDT_KERNEL_ENTRY_DATA GDT_ENTRY(0, 0xffffffff, GDT_FLAG_FOUR_KILOBYTE_GRANULARITY | GDT_FLAG_64BIT_MODE, GDT_ACCESS_PRESENT | GDT_ACCESS_PRIVILEGE_RING0 | GDT_ACCESS_DESCRIPTOR_DATA | GDT_ACCESS_READABLE_WRITABLE)

// Constants.
#define GDT_ALIGNMENT 0x1000
#define GDT_SIZE 0x800