#include "types.h"

#pragma once

class GDT {

    public:
    class SegmentDescriptor {
        
        private:
        // Low bytes of limit.
        uint16_t lLimit;

        // Low bytes of base.
        uint16_t lBase;

        // Mid byte of base.
        uint8_t mBase;

        // Access rights byte.
        uint8_t access;
        
        // Limit high 4 bits and 4 bits for flags.
        uint8_t hLimit;

        // High bytes of base.
        uint8_t hBase;

        public:
        SegmentDescriptor(uint32_t base, uint32_t limit, uint8_t flags);
        ~SegmentDescriptor();

        // Getters.
        uint32_t Base();
        uint32_t Limit();
        uint8_t Access();
        uint8_t Flags();

    } __attribute__((packed)); // GCC Compiler Directive - Pack the class so the compiler doesn't add fucking useless padding bytes.

    private:
    uint32_t size;
    uint16_t numEntries;
    SegmentDescriptor * table;

    public:
    GDT(void * gdt, uint16_t endIndex, uint32_t size);
    ~GDT();

    uint16_t CreateEntry(uint32_t base, uint32_t limit, uint8_t flags);
    SegmentDescriptor * GetEntry(uint16_t index);

};