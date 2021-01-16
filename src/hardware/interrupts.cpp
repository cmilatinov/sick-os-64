#include "hardware/interrupts.h"
#include "modules/lib.h"

IDT::GateDescriptor::GateDescriptor(){

    uint64_t * ptr = reinterpret_cast<uint64_t*>(this);

    // Zero the fields
    ptr[0] = 0;
    ptr[1] = 0;
}

IDT::GateDescriptor::GateDescriptor(uint64_t base, uint16_t selector, uint8_t typeAttr){
    this->lBase = base & 0xFFFF;
    this->selector = selector;
    this->typeAttr = typeAttr;
    this->mBase = (base >> 16) & 0xFFFF;
    this->hBase = (base >> 32) & 0xFFFFFFFF;
}

uint64_t IDT::GateDescriptor::Base(){
    uint64_t result = static_cast<uint64_t>(hBase) << 32;
    result |= mBase << 16;
    result |= lBase;
    return result;
}

uint16_t IDT::GateDescriptor::Selector(){
    return selector;
}

uint8_t IDT::GateDescriptor::TypeAttributes(){
    return typeAttr;
}

void IDT::SetEntry(uint8_t index, void (* handler)(), uint16_t selector, uint8_t typeAttr){
    table[index] = GateDescriptor(reinterpret_cast<uint64_t>(handler), selector, IDT_PRESENT | typeAttr);
}

IDT::GateDescriptor * IDT::GetEntry(uint8_t index){
    return table + index;
}

void IDT::LoadIDT(){

    // 80 Bit integer
    uint8_t value[10];

    // 16 Bit IDT size.
    *reinterpret_cast<uint16_t*>(value) = IDT_SIZE * IDT_ENTRY_SIZE - 1;

    // 64 Bit pointer to the IDT.
    *reinterpret_cast<uint64_t*>(&reinterpret_cast<uint16_t*>(value)[1]) = reinterpret_cast<uint64_t>(table);

    // Load the IDT in assembly.
    asm volatile(
        "lidt %0"
        :
        : "m" (value)
    );

}

InterruptManager * InterruptManager::activeManager = 0;

InterruptManager::InterruptManager() :
    picMasterCommandPort(PIC1_COMMAND_PORT),
    picMasterDataPort(PIC1_DATA_PORT),
    picSlaveCommandPort(PIC2_COMMAND_PORT),
    picSlaveDataPort(PIC2_DATA_PORT)
{
    this->codeSegment = KERNEL_GDT_ENTRY * GDT_ENTRY_SIZE;

    for(uint32_t i = 0; i < IDT_SIZE; i++){
        idt.SetEntry(i, &IgnoreInterrupt, codeSegment, IDT_PRIVILEGE0 | IDT_INTERRUPT_GATE);
        this->handlers[i] = nullptr;
    }

    idt.SetEntry(0x00, &HandleException0x00, codeSegment, IDT_PRIVILEGE0 | IDT_INTERRUPT_GATE);
    idt.SetEntry(0x01, &HandleException0x01, codeSegment, IDT_PRIVILEGE0 | IDT_INTERRUPT_GATE);
    idt.SetEntry(0x02, &HandleException0x02, codeSegment, IDT_PRIVILEGE0 | IDT_INTERRUPT_GATE);
    idt.SetEntry(0x03, &HandleException0x03, codeSegment, IDT_PRIVILEGE0 | IDT_INTERRUPT_GATE);
    idt.SetEntry(0x04, &HandleException0x04, codeSegment, IDT_PRIVILEGE0 | IDT_INTERRUPT_GATE);
    idt.SetEntry(0x05, &HandleException0x05, codeSegment, IDT_PRIVILEGE0 | IDT_INTERRUPT_GATE);
    idt.SetEntry(0x06, &HandleException0x06, codeSegment, IDT_PRIVILEGE0 | IDT_INTERRUPT_GATE);
    idt.SetEntry(0x07, &HandleException0x07, codeSegment, IDT_PRIVILEGE0 | IDT_INTERRUPT_GATE);
    idt.SetEntry(0x08, &HandleException0x08, codeSegment, IDT_PRIVILEGE0 | IDT_INTERRUPT_GATE);
    idt.SetEntry(0x09, &HandleException0x09, codeSegment, IDT_PRIVILEGE0 | IDT_INTERRUPT_GATE);
    idt.SetEntry(0x0A, &HandleException0x0A, codeSegment, IDT_PRIVILEGE0 | IDT_INTERRUPT_GATE);
    idt.SetEntry(0x0B, &HandleException0x0B, codeSegment, IDT_PRIVILEGE0 | IDT_INTERRUPT_GATE);
    idt.SetEntry(0x0C, &HandleException0x0C, codeSegment, IDT_PRIVILEGE0 | IDT_INTERRUPT_GATE);
    idt.SetEntry(0x0D, &HandleException0x0D, codeSegment, IDT_PRIVILEGE0 | IDT_INTERRUPT_GATE);
    idt.SetEntry(0x0E, &HandleException0x0E, codeSegment, IDT_PRIVILEGE0 | IDT_INTERRUPT_GATE);
    idt.SetEntry(0x0F, &HandleException0x0F, codeSegment, IDT_PRIVILEGE0 | IDT_INTERRUPT_GATE);
    idt.SetEntry(0x10, &HandleException0x10, codeSegment, IDT_PRIVILEGE0 | IDT_INTERRUPT_GATE);
    idt.SetEntry(0x11, &HandleException0x11, codeSegment, IDT_PRIVILEGE0 | IDT_INTERRUPT_GATE);
    idt.SetEntry(0x12, &HandleException0x12, codeSegment, IDT_PRIVILEGE0 | IDT_INTERRUPT_GATE);
    idt.SetEntry(0x13, &HandleException0x13, codeSegment, IDT_PRIVILEGE0 | IDT_INTERRUPT_GATE);

    // Timer IRQ
    idt.SetEntry(IRQ_OFFSET_X86_64 + IRQ_TIMER, &HandleInterruptRequest0x00, codeSegment, IDT_PRIVILEGE0 | IDT_INTERRUPT_GATE);
    
    // Keyboard IRQ
    idt.SetEntry(IRQ_OFFSET_X86_64 + IRQ_KEYBOARD, &HandleInterruptRequest0x01, codeSegment, IDT_PRIVILEGE0 | IDT_INTERRUPT_GATE);
    
    // Mouse IRQ
    idt.SetEntry(IRQ_OFFSET_X86_64 + IRQ_MOUSE, &HandleInterruptRequest0x0C, codeSegment, IDT_PRIVILEGE0 | IDT_INTERRUPT_GATE);

    // Restart PICs.
    picMasterCommandPort.Write(0x11);
    picSlaveCommandPort.Write(0x11);

    // Remap PICs to proper interrupt vector offsets.
    picMasterDataPort.Write(IRQ_OFFSET_X86_64);
    picSlaveDataPort.Write(IRQ_OFFSET_X86_64 + 8);

    picMasterDataPort.Write(0x04);
    picSlaveDataPort.Write(0x02);

    picMasterDataPort.Write(0x01);
    picSlaveDataPort.Write(0x01);

    picMasterDataPort.Write(0x00);
    picSlaveDataPort.Write(0x00);

    idt.LoadIDT();
}

InterruptManager::~InterruptManager(){
    Deactivate();
}

void InterruptManager::Activate(){
    if(activeManager != nullptr)
        activeManager->Deactivate();
    
    activeManager = this;
    asm("sti");
}

void InterruptManager::Deactivate(){
    if(activeManager == this){
        activeManager = nullptr;
        asm("cli");
    }
}

uint32_t InterruptManager::AssignIRQ(uint8_t irq, InterruptHandler * handler){
    if (handlers[IRQ_OFFSET_X86_64 + irq] == nullptr) {
        handlers[IRQ_OFFSET_X86_64 + irq] = handler;
        return IRQ_OK;
    }
    return IRQ_IN_USE;
}

uint32_t InterruptManager::RequestIRQ(uint8_t irq, InterruptHandler * handler) {
    if (activeManager != nullptr)
        return activeManager->AssignIRQ(irq, handler);

    return IRQ_NOT_INITIALIZED;
}

void InterruptManager::SetMask(uint8_t irq, bool enable) {
    
    uint8_t port, bit, mask;
    if (irq >= 8) {
        bit = 1 << (irq - 8);
        port = PIC2_DATA_PORT;
    } else {
        bit = 1 << irq;
        port = PIC1_DATA_PORT;
    }

    mask = Port8::Read(port);

    /* A '1' bit in the mask inhibits the interrupt. */
    if (enable)
        mask &= ~bit;
    else
        mask |= bit;

    Port8::Write(port, mask);
    
}

// Single Interrupt Handler
extern "C" uint64_t HandleInterrupt(uint64_t rsp, uint8_t interruptNumber){
    if (InterruptManager::activeManager != nullptr)
        return InterruptManager::activeManager->HandleInterrupt(rsp, interruptNumber);
    return rsp;
}

uint64_t InterruptManager::HandleInterrupt(uint64_t rsp, uint8_t interruptNumber){

    // If this interrupt has a handler, handle it.
    if (handlers[interruptNumber] != nullptr)
        handlers[interruptNumber]->HandleInterrupt(interruptNumber);

    // Acknowledge hardware interrupts.
    if (interruptNumber >= IRQ_OFFSET_X86_64 && interruptNumber < IRQ_OFFSET_X86_64 + 0x10) {
        picMasterCommandPort.Write(0x20);
        
        if(IRQ_OFFSET_X86_64 + 8 <= interruptNumber)
            picSlaveCommandPort.Write(0x20);
            
    }

    return rsp;

}