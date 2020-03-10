#include "drivers/keyboard.h"
#include "utils/lib.h"

KeyboardDriver::KeyboardDriver(): 
    data(KEYBOARD_DATA_PORT),
    command(KEYBOARD_COMMAND_PORT) 
{}

KeyboardDriver::~KeyboardDriver() {}

uint64_t KeyboardDriver::HandleInterrupt(uint64_t rsp) {

    static uint8_t escape = 0;
    static uint8_t ctrl = 0;
    static uint8_t shift = 0;
    static uint8_t alt = 0;

    // Read the key that was pressed.
    uint8_t key = data.Read();
    
    // Check if modifiers changed.
    switch(key) {
        case SC_LSHIFT: shift = 0xFF; break;
        case SC_RSHIFT: shift = 0xFF; break;
        case SC_LCTRL: ctrl = 0xFF; break;
        case SC_LALT: alt = 0xFF; break;

        case SC_RELEASE | SC_LSHIFT: shift = 0; break;
        case SC_RELEASE | SC_RSHIFT: shift = 0; break;
        case SC_RELEASE | SC_LCTRL: ctrl = 0; break;
        case SC_RELEASE | SC_LALT: alt = 0; break;
    }

    // Modifiers
    uint8_t mods = (ctrl & MOD_CTRL) | (alt & MOD_ALT) | (shift & MOD_SHIFT);

    // Press or release handler and fix scan code
    void (*handler)(char, uint8_t, uint8_t);
    if(key & SC_RELEASE) {
        handler = keyUp;
        key &= ~SC_RELEASE;
    } else
        handler = keyDown;

    // Virtual Key Code Mapping
    if(key == SC_ESCAPE_CODE) {
        escape = 0xFF;
    } else if(handler != nullptr) {
        switch(key) {

            // Letters
            case SC_A: handler(shift ? 'A' : 'a', VK_A, mods); break;
            case SC_B: handler(shift ? 'B' : 'b', VK_B, mods); break;
            case SC_C: handler(shift ? 'C' : 'c', VK_C, mods); break;
            case SC_D: handler(shift ? 'D' : 'd', VK_D, mods); break;
            case SC_E: handler(shift ? 'E' : 'e', VK_E, mods); break;
            case SC_F: handler(shift ? 'F' : 'f', VK_F, mods); break;
            case SC_G: handler(shift ? 'G' : 'g', VK_G, mods); break;
            case SC_H: handler(shift ? 'H' : 'h', VK_H, mods); break;
            case SC_I: handler(shift ? 'I' : 'i', VK_I, mods); break;
            case SC_J: handler(shift ? 'J' : 'j', VK_J, mods); break;
            case SC_K: handler(shift ? 'K' : 'k', VK_K, mods); break;
            case SC_L: handler(shift ? 'L' : 'l', VK_L, mods); break;
            case SC_M: handler(shift ? 'M' : 'm', VK_M, mods); break;
            case SC_N: handler(shift ? 'N' : 'n', VK_N, mods); break;
            case SC_O: handler(shift ? 'O' : 'o', VK_O, mods); break;
            case SC_P: handler(shift ? 'P' : 'p', VK_P, mods); break;
            case SC_Q: handler(shift ? 'Q' : 'q', VK_Q, mods); break;
            case SC_R: handler(shift ? 'R' : 'r', VK_R, mods); break;
            case SC_S: handler(shift ? 'S' : 's', VK_S, mods); break;
            case SC_T: handler(shift ? 'T' : 't', VK_T, mods); break;
            case SC_U: handler(shift ? 'U' : 'u', VK_U, mods); break;
            case SC_V: handler(shift ? 'V' : 'v', VK_V, mods); break;
            case SC_X: handler(shift ? 'X' : 'x', VK_X, mods); break;
            case SC_Y: handler(shift ? 'Y' : 'y', VK_Y, mods); break;
            case SC_Z: handler(shift ? 'Z' : 'z', VK_Z, mods); break;

            // Numbers
            case SC_0: handler(shift ? ')' : '0', VK_0, mods); break;
            case SC_1: handler(shift ? '!' : '1', VK_1, mods); break;
            case SC_2: handler(shift ? '@' : '2', VK_2, mods); break;
            case SC_3: handler(shift ? '#' : '3', VK_3, mods); break;
            case SC_4: handler(shift ? '$' : '4', VK_4, mods); break;
            case SC_5: handler(shift ? '%' : '5', VK_5, mods); break;
            case SC_6: handler(shift ? '^' : '6', VK_6, mods); break;
            case SC_7: handler(shift ? '&' : '7', VK_7, mods); break;
            case SC_8: handler(shift ? '*' : '8', VK_8, mods); break;
            case SC_9: handler(shift ? '(' : '9', VK_9, mods); break;
            
            // Special
            case SC_ESCAPE: handler('\0', VK_ESCAPE, mods); break;
            case SC_BACKTICK: handler(shift ? '~' : '`', VK_BACKTICK, mods); break;
            case SC_MINUS: handler(shift ? '_' : '-', VK_MINUS, mods); break;
            case SC_EQUALS: handler(shift ? '+' : '=', VK_EQUALS, mods); break;
            case SC_BACKSPACE: handler('\b', VK_BACKSPACE, mods); break;

            // Function Keys
            case SC_F1: handler('\0', VK_F1, mods); break;
            case SC_F2: handler('\0', VK_F2, mods); break;
            case SC_F3: handler('\0', VK_F3, mods); break;
            case SC_F4: handler('\0', VK_F4, mods); break;
            case SC_F5: handler('\0', VK_F5, mods); break;
            case SC_F6: handler('\0', VK_F6, mods); break;
            case SC_F7: handler('\0', VK_F7, mods); break;
            case SC_F8: handler('\0', VK_F8, mods); break;
            case SC_F9: handler('\0', VK_F9, mods); break;
            case SC_F10: handler('\0', VK_F10, mods); break;
            case SC_F11: handler('\0', VK_F11, mods); break;
            case SC_F12: handler('\0', VK_F12, mods); break;

            // Locks
            case SC_CAPSLOCK: handler('\0', VK_CAPSLOCK, mods); break;
            case SC_NUMLOCK: handler('\0', VK_NUMLOCK, mods); break;
            case SC_SCRLOCK: handler('\0', VK_SCRLOCK, mods); break;

            // Miscellaneous
            case SC_LSHIFT: handler('\0', VK_LSHIFT, mods); break;
            case SC_RSHIFT: handler('\0', VK_RSHIFT, mods); break;
            case SC_LCTRL: handler('\0', escape ? VK_RCTRL : VK_LCTRL, mods); break;
            case SC_LALT: handler('\0', escape ? VK_RALT : VK_LALT, mods); break;
            case SC_ENTER: handler('\n', VK_ENTER, mods); break;
            case SC_SPACE: handler(' ', VK_SPACE, mods); break;

            // Punctuation
            case SC_BRACKET_OPEN: handler(shift ? '{' : '[', VK_BRACKET_OPEN, mods); break;
            case SC_BRACKET_CLOSE: handler(shift ? '}' : ']', VK_BRACKET_CLOSE, mods); break;
            case SC_BACKSLASH: handler(shift ? '|' : '\\', VK_BACKSLASH, mods); break;
            case SC_SEMI_COLON: handler(shift ? ':' : ';', VK_SEMI_COLON, mods); break;
            case SC_SINGLE_QUOTE: handler(shift ? '"' : '\'', VK_SINGLE_QUOTE, mods); break;
            case SC_COMMA: handler(shift ? '<' : ',', VK_COMMA, mods); break;
            case SC_PERIOD: handler(shift ? '>' : '.', VK_PERIOD, mods); break;
            case SC_SLASH: handler(shift ? '?' : '/', VK_SLASH, mods); break;

            //default: printh(key);

        }
        escape = 0;
    }

    return rsp;

}

void KeyboardDriver::Activate() {

    // Read any data needed to be read
    // Bit 0 = There is data on port 0x60
    while(command.Read() & 0x1) 
        data.Read();

    // Controller Command 0xAE = Enable keyboard
    command.Write(0xAE); 
    
    // Controller Command 0x20 = Read controller command byte
    command.Write(0x20); 

    // Set controller command bit 0 and disable bit 3
    // Bit 0 = Keyboard interrupt enable
    // Bit 3 = Ignore keyboard lock
    uint8_t status = (data.Read() | 1) & ~0x10; 
    
    // Controller Command 0x60 = Write controller command byte
    command.Write(0x60); 

    // Write the command byte to data port
    data.Write(status); 

}

void KeyboardDriver::OnKeyDown(void (*keyDown) (char, uint8_t, uint8_t)) {
    this->keyDown = keyDown;
}

void KeyboardDriver::OnKeyUp(void (*keyUp) (char, uint8_t, uint8_t)) {
    this->keyUp = keyUp;
}