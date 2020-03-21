#pragma once

#include "common/types.h"

#define COLOR_BLACK             0
#define COLOR_BLUE              1
#define COLOR_GREEN             2
#define COLOR_CYAN              3
#define COLOR_RED               4
#define COLOR_MAGENTA           5
#define COLOR_BROWN             6
#define COLOR_LIGHT_GRAY        7
#define COLOR_DARK_GRAY         8
#define COLOR_LIGHT_BLUE        9
#define COLOR_LIGHT_GREEN       10
#define COLOR_LIGHT_CYAN        11
#define COLOR_LIGHT_RED         12
#define COLOR_LIGHT_MAGENTA     13
#define COLOR_YELLOW            14
#define COLOR_WHITE             15

class GraphicsDriver {

    public:
        GraphicsDriver() {}
        ~GraphicsDriver() {}

        virtual void Clear() = 0;
        virtual void Draw() = 0;

        virtual void PutPixel(uint32_t x, uint32_t y) = 0;
        virtual void FillRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

        virtual bool SupportsGraphicsMode(uint8_t mode) = 0;
        virtual bool SetGraphicsMode(uint8_t mode) = 0;

        virtual void SetClearColor(uint8_t color) = 0;
        virtual void SetColor(uint8_t color) = 0;

};