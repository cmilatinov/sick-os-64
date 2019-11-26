#include "gdt.h"

GDT::SegmentDescriptor::SegmentDescriptor(uint32_t base, uint32_t limit, uint8_t access){
    
    // Use 32-bit address space for limit.
    // 12 least significant bits are implicitly set to 1.
    this->hLimit = GDT_FLAG_FOUR_KILOBYTE_GRANULARITY | GDT_FLAG_64BIT_MODE;

    // Encode the limit.
    this->lLimit = (limit >> 12) & 0xFFFF;
    this->hLimit |= (limit >> 16) & 0xF;

    // Encode the base.
    this->lBase = base & 0xFFFF;
    this->mBase = (base >> 16) & 0xFF;
    this->hBase = (base >> 24) & 0xFF;

    // Encode the access byte.
    this->access = access;
}

GDT::SegmentDescriptor::~SegmentDescriptor(){};

uint32_t GDT::SegmentDescriptor::Base(){

    // Decode the base.
    uint32_t result = hBase;
    result = (result << 8) | mBase;
    result = (result << 16) | lBase;

    // Return the result.
    return result;
}

uint32_t GDT::SegmentDescriptor::Limit(){

    // Decode the limit.
    uint32_t result = hLimit & 0xF;
    result = (result << 16) | lLimit;

    // Add the implicit bits.
    result = (result << 12) | 0xFFF;

    // Return the result.
    return result;
}

uint8_t GDT::SegmentDescriptor::Access(){

    // Return the access byte.
    return access;
}

uint8_t GDT::SegmentDescriptor::Flags(){
    
    // Return the 4 high bits of the 7th byte as the flags.
    return hLimit >> 4;
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
    
    return NULL;
}
