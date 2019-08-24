#include "gdt.h"

GDT::SegmentDescriptor::SegmentDescriptor(uint32_t base, uint32_t limit, uint8_t flags){

    // Byte pointer to segment descriptor.
    uint8_t * ptr = reinterpret_cast<uint8_t *>(this);

    if(limit <= UINT16_MAX)

        // Use 16-bit address space for limit.
        ptr[6] = 0b01000000;

    else{

        // Use 32-bit address space for limit.
        // 12 least significant bits are implicitly set to 1.
        ptr[6] = 0b11000000;

        // Discard the 12 least significant bits.
        // Compensate by subtracting 1 from the shifted limit if those 12 bits aren't all ones.
        if((limit & 0xFFF) != 0xFFF)
            limit = (limit >> 12) - 1;
        else
            limit = limit >> 12;
        
    }

    // Encode the limit.
    reinterpret_cast<uint16_t *>(this)[0] = limit & 0xFFFF;
    ptr[6] |= (limit >> 16) & 0xF;

    // Encode the base.
    reinterpret_cast<uint16_t *>(this)[1] = base & 0xFFFF;
    ptr[4] = (base >> 16) & 0xFF;
    ptr[7] = (base >> 24) & 0xFF;

    // Encode the flags.
    ptr[5] = flags;
}

GDT::SegmentDescriptor::~SegmentDescriptor(){};

uint32_t GDT::SegmentDescriptor::Base(){

    // Byte pointer to segment descriptor.
    uint8_t * ptr = reinterpret_cast<uint8_t *>(this);

    // Decode the base.
    uint32_t result = ptr[7];
    result = (result << 8) | ptr[4];
    result = (result << 16) | reinterpret_cast<uint16_t *>(this)[1];

    // Return the result.
    return result;
}

uint32_t GDT::SegmentDescriptor::Limit(){
    
    // Byte pointer to segment descriptor.
    uint8_t * ptr = reinterpret_cast<uint8_t *>(this);

    // Decode the limit.
    uint32_t result = ptr[6] & 0xF;
    result = (result << 16) | reinterpret_cast<uint16_t *>(this)[0];

    // Add the implicit bits if necessary.
    if(ptr[6] & 0b10000000)
        result = (result << 12) | 0xFFF;

    // Return the result.
    return result;
}

uint8_t GDT::SegmentDescriptor::Access(){
    
    // Byte pointer to segment descriptor.
    uint8_t * ptr = reinterpret_cast<uint8_t *>(this);

    // Return the access byte.
    return ptr[5];
}

uint8_t GDT::SegmentDescriptor::Flags(){
    
    // Byte pointer to segment descriptor.
    uint8_t * ptr = reinterpret_cast<uint8_t *>(this);

    // Return the 4 high bits of the 7th byte as the flags.
    return ptr[6] >> 4;
}


GDT::GDT(void * gdt, uint16_t endIndex, uint32_t size){
    this->table = reinterpret_cast<SegmentDescriptor*>(gdt);
    this->size = size;
    this->numEntries = endIndex + 1;
};

GDT::~GDT(){};

uint16_t GDT::CreateEntry(uint32_t base, uint32_t limit, uint8_t flags){

    if(numEntries * sizeof(SegmentDescriptor) < size){
        table[numEntries] = SegmentDescriptor(base, limit, flags);
        return numEntries++;
    }

    return UINT16_MAX;

}

GDT::SegmentDescriptor * GDT::GetEntry(uint16_t index){
    if(index < numEntries)
        return table + index;
    
    return nullptr;
}
