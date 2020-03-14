#pragma once

#include "common/types.h"

union Color {
    struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a = 0xFF;
    };
    struct {
        uint32_t color;
    };

    // RGB
    static const Color RED;
    static const Color GREEN;
    static const Color BLUE;

    // White / Grey / Black
    static const Color WHITE;
    static const Color GRAY;
    static const Color BLACK;

    // Others
    static const Color ORANGE;
    static const Color YELLOW;
    static const Color PURPLE;
    static const Color BROWN;
    static const Color MAGENTA;
    static const Color CYAN;
    static const Color LIME;
    static const Color VIOLET;
    static const Color PINK;

} __attribute__((packed));

class GraphicsDriver {

    protected:
        Color clearColor = Color::BLACK;
        Color currentColor = Color::WHITE;

    public:
        GraphicsDriver() {}
        virtual ~GraphicsDriver();

        virtual void Clear() = 0;

        virtual void PutPixel(uint32_t x, uint32_t y) = 0;
        virtual void FillRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

        virtual bool SupportsGraphicsMode(uint32_t width, uint32_t height, uint32_t colorDepth) = 0;
        virtual void SetGraphicsMode(uint32_t width, uint32_t height, uint32_t colorDepth) = 0;

        virtual inline void SetClearColor(Color color) final {
            clearColor = color;
        };
        virtual inline void SetColor(Color color) final {
            currentColor = color;
        };

};