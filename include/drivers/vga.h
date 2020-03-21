#pragma once

#include "hardware/ports.h"

#include "modules/graphics.h"
#include "modules/memory.h"

#define MISC                0x3C2

#define CRTC_INDEX          0x3D4
#define CRTC_DATA           0x3D5
#define CRTC_NUM_REG        25

#define SEQ_INDEX           0x3C4
#define SEQ_DATA            0x3C5
#define SEQ_NUM_REG         5

#define GRAPHICS_INDEX      0x3CE
#define GRAPHICS_DATA       0x3CF
#define GRAPHICS_NUM_REG    9

#define ATTRIBUTE_INDEX     0x3C0
#define ATTRIBUTE_READ      0x3C1
#define ATTRIBUTE_WRITE     0x3C0
#define ATTRIBUTE_RESET     0x3DA
#define ATTRIBUTE_NUM_REG   21

#define DAC_MASK  			0x3C6
#define DAC_INDEX_READ  	0x3C7
#define DAC_INDEX_WRITE 	0x3C8
#define DAC_DATA  			0x3C9


#define VGA_MODE_320_200_256 	1

class VGADriver : public GraphicsDriver {

    private:
        static uint8_t * const MODE1_BUFFER;
		static const uint8_t MODE1_REGISTERS[];

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
		
        // Framebuffers
        uint8_t * displayFramebuffer;
        uint8_t * memoryFramebuffer;

		// Colors
		uint8_t currentColor = COLOR_WHITE;
		uint8_t clearColor = COLOR_BLACK;

		// Mode
		uint8_t videoMode = 0;
		
		// Display size
		uint32_t framebufferWidth;
		uint32_t framebufferHeight;

        void WriteRegisters(const uint8_t * registers);

    public:
        VGADriver();
        ~VGADriver();
		
        virtual void Clear() final;
		virtual void Draw() final;

        virtual void PutPixel(uint32_t x, uint32_t y) final;
        virtual void FillRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height) final;

        virtual bool SupportsGraphicsMode(uint8_t mode) final;
        virtual bool SetGraphicsMode(uint8_t mode) final;

		virtual void SetColor(uint8_t color) final;
		virtual void SetClearColor(uint8_t color) final;

};