#include "hardware/cpu.h"

CPU * CPU::activeCPU = nullptr;

CPU::CPU() {
    if(activeCPU != nullptr)
        delete activeCPU;
    activeCPU = this;
    DetectFeatures();
}

CPU::~CPU() {
    if(activeCPU == this)
        activeCPU = nullptr;
}

void CPU::DetectFeatures() {

    uint32_t ecx, edx;

    asm volatile (
        "movl $1, %%eax\n"
        "cpuid\n"
        "movl %%ecx, %0\n"
        "movl %%edx, %1"
        : "=r"(ecx), "=r"(edx)
        : 
        : "eax", "ecx", "edx"
    );
    
    cpuid = (static_cast<uint64_t>(edx) << 32) | ecx;

}

bool CPU::EnableSSE() {
    
    // No SSE support
    if(!HasFeature(CPU_FEATURE_SSE))
        return false;

    // Enable SSE
    asm volatile(
        // Copy CR0 to RAX
        "movq %%cr0, %%rax\n"

        // Clear coprocessor emulation CR0.EM
        "andw $0xFFFB, %%ax\n"

        // Set coprocessor monitoring CR0.MP
        "orw $0x2, %%ax\n"
        
        // Set new CR0 value
        "mov %%eax, %%cr0\n"
        
        // Copy CR4 to EAX
        "mov %%cr4, %%eax\n"

        // Set CR4.OSFXSR and CR4.OSXMMEXCPT at the same time
        "or $(3 << 9), %%ax\n"

        // Set new CR4 value 		
        "mov %%eax, %%cr4\n"

        :
        :
        : "rax"
    );

    return true;

}