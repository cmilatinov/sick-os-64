#include "types.h"
#include "arch/x84_64/kernel.h"

#pragma once

#define IDT_SIZE 256

// Interrupt Manager class
class InterruptManager {
    
    // Interrupt Descriptor Table
    class IDT {

        public:
        // IDT Entry
        class GateDescriptor {

            private:
            // Low bytes of base pointer.
            uint16_t lBase;

            // Code Segment Selector from GDT.
            uint16_t selector;

            // Set to zero always.
            uint8_t zero;

            // Type and attributes.
            uint8_t typeAttr;

            // High bytes of base pointer.
            uint16_t hBase;

            public:
            GateDescriptor(uint32_t base, uint16_t selector, uint8_t typeAttr);
            ~GateDescriptor();

            uint32_t Base();
            uint16_t Selector();
            uint8_t TypeAttributes();

        } __attribute__((packed));

        // Interrupt Handler
        class InterruptHandler {



        };

        private:
        GateDescriptor table[IDT_SIZE];

        public:
        void CreateEntry(uint8_t index, uint32_t base, uint16_t selector, uint8_t type);
        GateDescriptor GetEntry(uint8_t index);
        void LoadIDT();

    };



};