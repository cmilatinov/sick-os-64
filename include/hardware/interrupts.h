#pragma once

#include "arch/x84_64/kernel.h"
#include "arch/x84_64/gdt.h"

#include "common/types.h"

#include "hardware/ports.h"

#define IDT_ENTRY_SIZE 	16
#define IDT_SIZE 		256

#define IDT_INTERRUPT_GATE 	0xE
#define IDT_TRAP_GATE 		0xF
#define IDT_CALL_GATE 		0xC

#define IDT_STORAGE 	(1 << 4)

#define IDT_PRIVILEGE0 	(0x0 << 5)
#define IDT_PRIVILEGE1 	(0x1 << 5)
#define IDT_PRIVILEGE2 	(0x2 << 5)
#define IDT_PRIVILEGE3 	(0x3 << 5)

#define IDT_PRESENT 	(1 << 7)

#define PIC1_COMMAND_PORT 	0x20
#define PIC1_DATA_PORT 		0x21

#define PIC2_COMMAND_PORT   0xA0
#define PIC2_DATA_PORT      0xA1

#define NUM_EXCEPTION_VECTORS   0x20
#define IRQ_OFFSET_X86_64       NUM_EXCEPTION_VECTORS

#define IRQ_TIMER           0x00
#define IRQ_KEYBOARD        0x01
#define IRQ_MOUSE           0x0C

#define IRQ_OK                  0
#define IRQ_NOT_INITIALIZED     1
#define IRQ_IN_USE              2

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

        uint64_t Base();
        uint16_t Selector();
        uint8_t TypeAttributes();

    } PACKED;

    private:
    GateDescriptor table[IDT_SIZE];

    public:
    void SetEntry(uint8_t index, void (* handler)(), uint16_t selector, uint8_t typeAttr);
    GateDescriptor * GetEntry(uint8_t index);
    void LoadIDT();

};

// Interrupt Handler Class
class InterruptHandler {

    public:
    virtual void HandleInterrupt(uint8_t vector) = 0;

};

// Register data saved when an interrupt occurs
struct InterruptContext {
    uint64_t rax, rbx, rcx, rdx, rbp, rsp, rsi, rdi;
    uint64_t r8, r9, r10, r11, r12, r13, r14, r15;
    uint64_t rip, cs, rflags;
} PACKED;

// Assembly definitions
extern "C" {

    // This is called from assembly
    uint64_t HandleInterrupt(uint64_t rsp, uint8_t interruptNumber);

    // Ignore Interrupt Request
    void IgnoreInterrupt();

    // Interrupt Requests
    void HandleInterruptRequest0x00(); // Timer
    void HandleInterruptRequest0x01(); // Keyboard
    void HandleInterruptRequest0x0C(); // Mouse

    // Exceptions
    void HandleException0x00();
    void HandleException0x01();
    void HandleException0x02();
    void HandleException0x03();
    void HandleException0x04();
    void HandleException0x05();
    void HandleException0x06();
    void HandleException0x07();
    void HandleException0x08();
    void HandleException0x09();
    void HandleException0x0A();
    void HandleException0x0B();
    void HandleException0x0C();
    void HandleException0x0D();
    void HandleException0x0E();
    void HandleException0x0F();
    void HandleException0x10();
    void HandleException0x11();
    void HandleException0x12();
    void HandleException0x13();

    // Save and restore contexts
    uint32_t SaveContext(InterruptContext * ctx);
    void RestoreContext(InterruptContext * ctx);

}

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
    const Port8S picMasterCommandPort = Port8S(PIC1_COMMAND_PORT);
    const Port8S picMasterDataPort = Port8S(PIC1_DATA_PORT);
    const Port8S picSlaveCommandPort = Port8S(PIC2_COMMAND_PORT);
    const Port8S picSlaveDataPort = Port8S(PIC2_DATA_PORT);

    // GDT Code Segment Index
    uint32_t codeSegment;

    // Instance based interrupt handler
    uint64_t HandleInterrupt(uint64_t rsp, uint8_t interruptNumber);

    // Request to set an IRQ handler
    uint32_t AssignIRQ(uint8_t irq, InterruptHandler * handler);

    public:
    InterruptManager();
    ~InterruptManager();

    // Turn an interrupt manager on or off
    void Activate();
    void Deactivate();

    // Make interrupt handler friend so it can access the active interrupt manager
    friend uint64_t ::HandleInterrupt(uint64_t rsp, uint8_t interruptNumber);

    static uint32_t RequestIRQ(uint8_t irq, InterruptHandler * handler);
    static void SetMask(uint8_t irq, bool enable);

};

namespace Interrupts {

    inline uint32_t SaveContext(InterruptContext * ctx) {
        return ::SaveContext(ctx);
    }
    inline void RestoreContext(InterruptContext * ctx) {
        ::RestoreContext(ctx);
    }

    inline void Halt() {
        asm volatile ("hlt");
    }

}