#pragma once

#include "common/types.h"

#include "modules/memory.h"

#define MULTIBOOT_INFO_TAG_ALIGNMENT     0x8

#define MULTIBOOT_INFO_TAG_MEMORY        4
#define MULTIBOOT_INFO_TAG_FRAMEBUFFER   8

#define MULTIBOOT_FRAMEBUFFER_TYPE_PALETTE    0
#define MULTIBOOT_FRAMEBUFFER_TYPE_RGB        1
#define MULTIBOOT_FRAMEBUFFER_TYPE_TEXT       2

// Boot info header
struct MultibootInfoHeader {
    uint32_t size;
    uint32_t reserved;
} PACKED;

// General tag header struct
struct MultibootInfoTagHeader {
    uint32_t type;
    uint32_t size;
} PACKED;

// Gives memory sizes
struct MultibootInfoMemory {
    uint32_t sizeLower;
    uint32_t sizeUpper;
} PACKED;

// Gives VRAM information
struct MultibootInfoFramebuffer {
    
    uint64_t address;
    uint32_t pitch;
    uint32_t width;
    uint32_t height;
    uint8_t bpp;
    uint8_t type;
    uint8_t reserved;

    struct Palette {

        struct Color {
            uint8_t r;
            uint8_t g;
            uint8_t b;
        } PACKED;

        uint32_t numColors;
        Color * colors;

    } PACKED;

    struct RGBColor {
        uint8_t rPos;
        uint8_t rSize;
        uint8_t gPos;
        uint8_t gSize;
        uint8_t bPos;
        uint8_t bSize;
    } PACKED;

} PACKED;


// Used to parse the multiboot information
class MultibootLoader {

    private:
        MultibootInfoMemory memory;

        MultibootInfoFramebuffer framebuffer;
        MultibootInfoFramebuffer::RGBColor rgb;
        MultibootInfoFramebuffer::Palette palette;

        void Parse(void * multiboot);

    public:
        MultibootLoader(void * multiboot);
        ~MultibootLoader();

        void GetMemoryInfo(MultibootInfoMemory * dest);

        void GetFramebufferInfo(MultibootInfoFramebuffer * dest);
        bool GetFramebufferRGB(MultibootInfoFramebuffer::RGBColor * dest);
        bool GetFramebufferPalette(MultibootInfoFramebuffer::Palette * dest);

};