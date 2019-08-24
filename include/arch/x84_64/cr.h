// Control registers bit flags.
#pragma once

#define CR0_PM_ENABLE (1 << 0)      // Enable Protected Mode
#define CR0_PG_ENABLE (1 << 31)     // Enable Paging
#define CR0_ET (1 << 4)             // i387 Math Coprocessor Type

#define CR4_PSE_ENABLE (1 << 4)     // Enable Page Size Extension
#define CR4_PAE_ENABLE (1 << 5)     // Enable Physical Address Extension