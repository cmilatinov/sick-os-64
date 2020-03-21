#include "drivers/vga.h"

uint8_t * const VGADriver::MODE1_BUFFER = reinterpret_cast<uint8_t*>(0xA0000);
const uint8_t VGADriver::MODE1_REGISTERS[] = {
    /* MISC */
    0x63,
    /* SEQ */
    0x03, 0x01, 0x0F, 0x00, 0x0E,
    /* CRTC */
    0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
    0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x9C, 0x0E, 0x8F, 0x28, 0x40, 0x96, 0xB9, 0xA3,
    0xFF,
    /* GC */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
    0xFF,
    /* AC */
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x41, 0x00, 0x0F, 0x00, 0x00
};

VGADriver::VGADriver() {}

VGADriver::~VGADriver() {
    if (memoryFramebuffer != nullptr)
        free(memoryFramebuffer);
}

void VGADriver::WriteRegisters(const uint8_t * registers){

    // Miscellaneous Register
    misc.Write(*(registers++));
    
    // Sequencer Registers
    for (uint8_t i = 0; i < SEQ_NUM_REG; i++) {
        sequencerIndex.Write(i);
        sequencerData.Write(*(registers++));
    }
    
    // CRTC Registers
    uint8_t crtcRegisters[CRTC_NUM_REG];
    memcpy(registers, crtcRegisters, CRTC_NUM_REG);

    // Unlock CRTC Registers
    crtcIndex.Write(0x03);
    crtcData.Write(crtcData.Read() | 0x80);
    crtcIndex.Write(0x11);
    crtcData.Write(crtcData.Read() & 0x7F);
    
    // Make sure they stay unlocked
    crtcRegisters[0x3] = crtcRegisters[0x3] | 0x80;
    crtcRegisters[0x11] = crtcRegisters[0x11] & 0x7F;
    
    // Set CRTC registers
    for (uint8_t i = 0; i < CRTC_NUM_REG; i++) {
        crtcIndex.Write(i);
        crtcData.Write(crtcRegisters[i]);
    }

    registers += CRTC_NUM_REG;
    
    // Graphics controller registers
    for (uint8_t i = 0; i < GRAPHICS_NUM_REG; i++) {
        graphicsIndex.Write(i);
        graphicsData.Write(*(registers++));
    }
    
    // Attribute controller registers
    for (uint8_t i = 0; i < ATTRIBUTE_NUM_REG; i++) {
        attributeReset.Read();
        attributeIndex.Write(i);
        attributeWrite.Write(*(registers++));
    }
    
    attributeReset.Read();
    attributeIndex.Write(0x20);

}

void VGADriver::Clear() {
    memset_f(memoryFramebuffer, framebufferHeight * framebufferWidth, clearColor);
}

void VGADriver::Draw() {
    memcpy_f(memoryFramebuffer, displayFramebuffer, framebufferWidth * framebufferHeight);
}

void VGADriver::PutPixel(uint32_t x, uint32_t y) {
    memoryFramebuffer[y * framebufferWidth + x] = currentColor;
}

void VGADriver::FillRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    for (uint32_t i = y; i < y + height; i++) 
        memset_f(memoryFramebuffer + (i * framebufferWidth) + x, width, currentColor);
}

bool VGADriver::SupportsGraphicsMode(uint8_t mode) {
    return mode == VGA_MODE_320_200_256;
}

bool VGADriver::SetGraphicsMode(uint8_t mode) {

    if(!SupportsGraphicsMode(mode))
        return false;

    if (memoryFramebuffer != nullptr)
        free(memoryFramebuffer);

    switch (mode) {
        
        case VGA_MODE_320_200_256:
            framebufferWidth = 320;
            framebufferHeight = 200;
            displayFramebuffer = MODE1_BUFFER;
            memoryFramebuffer = reinterpret_cast<uint8_t*>(malloc(framebufferWidth * framebufferHeight));
            WriteRegisters(MODE1_REGISTERS);
            break;

    }

    videoMode = mode;

    return true;
}

void VGADriver::SetColor(uint8_t color) {
    currentColor = color;
}

void VGADriver::SetClearColor(uint8_t color) {
    clearColor = color;
}