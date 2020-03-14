#pragma once

#include "hardware/ports.h"
#include "modules/graphics.h"

#include "vga_reg.h"

class VGADriver : public GraphicsDriver {

    private:
        // Miscellaneous port
        Port8 misc = Port8(MISC);

        // Cathode Ray Tube Controller ports
        Port8 crtcIndex = Port8(CRTC_INDEX);
        Port8 crtcData = Port8(CRTC_INDEX);

        // Sequencer ports
        Port8 sequencerIndex = Port8(SEQ_INDEX);
        Port8 sequencerData = Port8(SEQ_DATA);

        // Graphics Controller ports
        Port8 graphicsIndex = Port8(GRAPHICS_INDEX);
        Port8 graphicsData = Port8(GRAPHICS_DATA);

        // Attribute Controller ports
        Port8 attributeIndex = Port8(ATTRIBUTE_INDEX);
        Port8 attributeRead = Port8(ATTRIBUTE_READ);
        Port8 attributeWrite = Port8(ATTRIBUTE_WRITE);
        Port8 attributeReset = Port8(ATTRIBUTE_RESET);

        inline void PutPixel(uint32_t x, uint32_t y, uint8_t colorIndex);
        inline uint8_t GetColorIndex(uint8_t r, uint8_t g, uint8_t b);

        void WriteRegisters(const uint8_t * registers);

    public:
        VGADriver();
        virtual ~VGADriver();

        virtual void Clear() final;

        virtual void PutPixel(uint32_t x, uint32_t y) final;
        virtual void FillRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height) final;

        virtual bool SupportsGraphicsMode(uint32_t width, uint32_t height, uint32_t colorDepth) final;
        virtual void SetGraphicsMode(uint32_t width, uint32_t height, uint32_t colorDepth) final;

};