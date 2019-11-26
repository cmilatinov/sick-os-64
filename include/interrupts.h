#include "types.h"
#include "ports.h"
#include "gdt.h"
#include "arch/x84_64/kernel.h"

#pragma once

#define IDT_ENTRY_SIZE 16
#define IDT_SIZE 256

#define IDT_INTERRUPT_GATE 0xE
#define IDT_TRAP_GATE 0xF
#define IDT_CALL_GATE 0xC

#define IDT_STORAGE (1 << 4)

#define IDT_PRIVILEGE0 (0x0 << 5)
#define IDT_PRIVILEGE1 (0x1 << 5)
#define IDT_PRIVILEGE2 (0x2 << 5)
#define IDT_PRIVILEGE3 (0x3 << 5)

#define IDT_PRESENT (1 << 7)

#define PIC1_COMMAND_PORT 0x20
#define PIC1_DATA_PORT 0x21

#define PIC2_COMMAND_PORT 0xA0
#define PIC2_DATA_PORT 0xA1

#define IRQ_OFFSET_X86_64 0x20

// Interrupt Descriptor Table Class
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
        uint8_t zero1 = 0;

        // Type and attributes.
        uint8_t typeAttr;
        
        // Middle bytes of base pointer.
        uint16_t mBase;

        // High bytes of base pointer.
        uint32_t hBase;

        // Set to zero always.
        uint32_t zero2 = 0;

        public:
        GateDescriptor();
        GateDescriptor(uint64_t base, uint16_t selector, uint8_t typeAttr);
        ~GateDescriptor();

        uint64_t Base();
        uint16_t Selector();
        uint8_t TypeAttributes();

    } __attribute__((packed));

    private:
    GateDescriptor table[IDT_SIZE];

    public:
    IDT();
    ~IDT();
    
    void SetEntry(uint8_t index, void (* handler)(), uint16_t selector, uint8_t typeAttr);
    GateDescriptor * GetEntry(uint8_t index);
    void LoadIDT();

};

// Interrupt Handler Class
class InterruptHandler {
    
    public:
    InterruptHandler();
    ~InterruptHandler();
    
    virtual uint64_t HandleInterrupt(uint64_t rsp);

};

// This is called from assembly
extern "C" uint64_t HandleInterrupt(uint64_t rsp, uint8_t interruptNumber);

// Ignore Interrupt Request
extern "C" void IgnoreInterrupt();

// Interrupt Requests
extern "C" void HandleInterruptRequest0x00(); // Timer
extern "C" void HandleInterruptRequest0x01(); // Keyboard

// Exceptions
extern "C" void HandleException0x00();
extern "C" void HandleException0x01();
extern "C" void HandleException0x02();
extern "C" void HandleException0x03();
extern "C" void HandleException0x04();
extern "C" void HandleException0x05();
extern "C" void HandleException0x06();
extern "C" void HandleException0x07();
extern "C" void HandleException0x08();
extern "C" void HandleException0x09();
extern "C" void HandleException0x0A();
extern "C" void HandleException0x0B();
extern "C" void HandleException0x0C();
extern "C" void HandleException0x0D();
extern "C" void HandleException0x0E();
extern "C" void HandleException0x0F();
extern "C" void HandleException0x10();
extern "C" void HandleException0x11();
extern "C" void HandleException0x12();
extern "C" void HandleException0x13();

// Interrupt Manager class
class InterruptManager {

    friend class InterruptHandler;

    private:
    // Current active interrupt manager
    static InterruptManager * activeManager;

    // Interrupt Table
    IDT idt;

    // Interrupt Handlers
    InterruptHandler * handlers[IDT_SIZE];

    // Programmable Interrupt Controller Ports
    Port8S picMasterCommandPort;
    Port8S picMasterDataPort;
    Port8S picSlaveCommandPort;
    Port8S picSlaveDataPort;

    // IRQ Interrupts Offset
    uint64_t hardwareInterruptOffset;

    // GDT Code Segment Index
    uint32_t codeSegment;

    public:
    InterruptManager(uint64_t hardwareOffset);
    ~InterruptManager();

    // Turn an interrupt manager on or off
    void Activate();
    void Deactivate();

    // Set handler
    void SetInterruptHandler(uint8_t interruptNumber, InterruptHandler * handler);

    // Instance based interrupt handler.
    uint64_t HandleInterrupt(uint64_t rsp, uint8_t interruptNumber);

    // Make interrupt handler friend so it can access the active interrupt manager.
    friend uint64_t ::HandleInterrupt(uint64_t rsp, uint8_t interruptNumber);

};