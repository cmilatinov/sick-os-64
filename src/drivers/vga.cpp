#include "drivers/vga.h"

VGADriver::VGADriver() {}

VGADriver::~VGADriver() {}

void VGADriver::WriteRegisters(const uint8_t * registers){

    // Miscellaneous Register
    misc.Write(*(registers++));
    
    // Sequencer Registers
    for(uint8_t i = 0; i < 5; i++) {
        sequencerIndex.Write(i);
        sequencerData.Write(*(registers++));
    }
    
    // CRTC Registers
    crtcIndex.Write(0x03);
    crtcData.Write(crtcData.Read() | 0x80);
    crtcIndex.Write(0x11);
    crtcData.Write(crtcData.Read() & ~0x80);
    
    registers[0x03] = registers[0x03] | 0x80;
    registers[0x11] = registers[0x11] & ~0x80;
    
    for(uint8_t i = 0; i < 25; i++)
    {
        crtcIndexPort.Write(i);
        crtcDataPort.Write(*(registers++));
    }
    
    // graphics controller
    for(uint8_t i = 0; i < 9; i++)
    {
        graphicsControllerIndexPort.Write(i);
        graphicsControllerDataPort.Write(*(registers++));
    }
    
    // attribute controller
    for(uint8_t i = 0; i < 21; i++)
    {
        attributeReset.Read();
        attributeIndex.Write(i);
        attributeWrite.Write(*(registers++));
    }
    
    attributeControllerResetPort.Read();
    attributeControllerIndexPort.Write(0x20);

}