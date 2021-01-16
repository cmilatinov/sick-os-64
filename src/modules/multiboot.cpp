#include "modules/multiboot.h"

MultibootLoader::MultibootLoader(void * multiboot) {
    Parse(multiboot);
}

MultibootLoader::~MultibootLoader() {}

void MultibootLoader::Parse(void * multiboot) {
    MultibootInfoHeader * header = reinterpret_cast<MultibootInfoHeader*>(multiboot);

    uint8_t * pointer = reinterpret_cast<uint8_t*>(multiboot) + sizeof(MultibootInfoHeader);
    while(pointer < reinterpret_cast<uint8_t*>(multiboot) + header->size) {
        
        // Basic tag header
        MultibootInfoTagHeader * tagHeader = reinterpret_cast<MultibootInfoTagHeader*>(pointer);

        switch(tagHeader->type) {

            case MULTIBOOT_INFO_TAG_MEMORY:

                // Upper and lower memory sizes
                memory = *reinterpret_cast<MultibootInfoMemory*>(pointer + sizeof(MultibootInfoTagHeader));

                break;
            
            case MULTIBOOT_INFO_TAG_FRAMEBUFFER:

                // Basic framebuffer info
                MultibootInfoFramebuffer * pFramebuffer = reinterpret_cast<MultibootInfoFramebuffer*>(pointer + sizeof(MultibootInfoTagHeader));
                framebuffer = *pFramebuffer;

                // Indexed color framebuffer info
                if (framebuffer.type == MULTIBOOT_FRAMEBUFFER_TYPE_PALETTE)
                    palette = { 
                        *reinterpret_cast<uint32_t*>(pFramebuffer + 1), 
                        reinterpret_cast<MultibootInfoFramebuffer::Palette::Color*>(reinterpret_cast<uint32_t*>(pFramebuffer + 1) + 1) 
                    };

                // RGB framebuffer color info
                else if(framebuffer.type == MULTIBOOT_FRAMEBUFFER_TYPE_RGB)
                    rgb = *reinterpret_cast<MultibootInfoFramebuffer::RGBColor*>(pFramebuffer + 1);
                
                break;

        }

        // Move to next tag, (current tag address + current tag size, padded to 8 byte alignment)
        pointer += tagHeader->size + (MULTIBOOT_INFO_TAG_ALIGNMENT - 1);
        pointer = reinterpret_cast<uint8_t*>(reinterpret_cast<uint64_t>(pointer) & ~(MULTIBOOT_INFO_TAG_ALIGNMENT - 1));

    }
}

void MultibootLoader::GetMemoryInfo(MultibootInfoMemory * dest) {
    *dest = memory;
}

void MultibootLoader::GetFramebufferInfo(MultibootInfoFramebuffer * dest) {
    *dest = framebuffer;
}

bool MultibootLoader::GetFramebufferRGB(MultibootInfoFramebuffer::RGBColor * dest) {
    if(framebuffer.type != MULTIBOOT_FRAMEBUFFER_TYPE_RGB)
        return false;

    *dest = rgb;
    return true;
}

bool MultibootLoader::GetFramebufferPalette(MultibootInfoFramebuffer::Palette * dest) {
    if(framebuffer.type != MULTIBOOT_FRAMEBUFFER_TYPE_PALETTE)
        return false;

    *dest = palette;
    return true;
}