#include "keyboard.h"

KeyboardEventHander::KeyboardEventHander(){}
KeyboardEventHander::~KeyboardEventHander(){}

void KeyboardEventHander::KeyDown(char c, uint8_t keycode, uint8_t modifiers){}
void KeyboardEventHander::KeyUp(char c, uint8_t keycode){}

KeyboardDriver::KeyboardDriver(KeyboardEventHander * handler): 
    data(KEYBOARD_DATA_PORT),
    command(KEYBOARD_COMMAND_PORT) 
{
    this->handler = handler;
}

KeyboardDriver::~KeyboardDriver(){}

uint64_t KeyboardDriver::HandleInterrupt(uint64_t rsp){

    static bool ctrl = false;
    static bool shift = false;
    static bool alt = false;

    // Read the key that was pressed.
    uint8_t key = data.Read();

    // If the handler is is not present, return.
    if(handler == NULL)
        return rsp;
    
    // Check if modifiers changed.
    switch(key){
        case SC_LSHIFT: shift = true; break;
        case SC_RSHIFT: shift = true; break;
        case SC_LCTRL: ctrl = true; break;
        case SC_LALT: alt = true; break;

        case SC_RELEASE | SC_LSHIFT: shift = false; break;
        case SC_RELEASE | SC_RSHIFT: shift = false; break;
        case SC_RELEASE | SC_LCTRL: ctrl = false; break;
        case SC_RELEASE | SC_LALT: alt = false; break;
    }

    // Modifiers
    uint8_t mods = 0;

    if(ctrl)
        mods |= MOD_CTRL;

    if(shift)
        mods |= MOD_SHIFT;

    if(alt)
        mods |= MOD_ALT;

    // Virtual Key Code Mapping
    switch(key) {

        // Letters
        case SC_A: handler->KeyDown(shift ? 'A' : 'a', VK_A, mods); break;
        case SC_B: handler->KeyDown(shift ? 'B' : 'b', VK_B, mods); break;
        case SC_C: handler->KeyDown(shift ? 'C' : 'c', VK_C, mods); break;
        case SC_D: handler->KeyDown(shift ? 'D' : 'd', VK_D, mods); break;
        case SC_E: handler->KeyDown(shift ? 'E' : 'e', VK_E, mods); break;
        case SC_F: handler->KeyDown(shift ? 'F' : 'f', VK_F, mods); break;
        case SC_G: handler->KeyDown(shift ? 'G' : 'g', VK_G, mods); break;
        case SC_H: handler->KeyDown(shift ? 'H' : 'h', VK_H, mods); break;
        case SC_I: handler->KeyDown(shift ? 'I' : 'i', VK_I, mods); break;
        case SC_J: handler->KeyDown(shift ? 'J' : 'j', VK_J, mods); break;
        case SC_K: handler->KeyDown(shift ? 'K' : 'k', VK_K, mods); break;
        case SC_L: handler->KeyDown(shift ? 'L' : 'l', VK_L, mods); break;
        case SC_M: handler->KeyDown(shift ? 'M' : 'm', VK_M, mods); break;
        case SC_N: handler->KeyDown(shift ? 'N' : 'n', VK_N, mods); break;
        case SC_O: handler->KeyDown(shift ? 'O' : 'o', VK_O, mods); break;
        case SC_P: handler->KeyDown(shift ? 'P' : 'p', VK_P, mods); break;
        case SC_Q: handler->KeyDown(shift ? 'Q' : 'q', VK_Q, mods); break;
        case SC_R: handler->KeyDown(shift ? 'R' : 'r', VK_R, mods); break;
        case SC_S: handler->KeyDown(shift ? 'S' : 's', VK_S, mods); break;
        case SC_T: handler->KeyDown(shift ? 'T' : 't', VK_T, mods); break;
        case SC_U: handler->KeyDown(shift ? 'U' : 'u', VK_U, mods); break;
        case SC_V: handler->KeyDown(shift ? 'V' : 'v', VK_V, mods); break;
        case SC_X: handler->KeyDown(shift ? 'X' : 'x', VK_X, mods); break;
        case SC_Y: handler->KeyDown(shift ? 'Y' : 'y', VK_Y, mods); break;
        case SC_Z: handler->KeyDown(shift ? 'Z' : 'z', VK_Z, mods); break;

        // Numbers
        case SC_0: handler->KeyDown(shift ? ')' : '0', VK_0, mods); break;
        case SC_1: handler->KeyDown(shift ? '!' : '1', VK_1, mods); break;
        case SC_2: handler->KeyDown(shift ? '@' : '2', VK_2, mods); break;
        case SC_3: handler->KeyDown(shift ? '#' : '3', VK_3, mods); break;
        case SC_4: handler->KeyDown(shift ? '$' : '4', VK_4, mods); break;
        case SC_5: handler->KeyDown(shift ? '%' : '5', VK_5, mods); break;
        case SC_6: handler->KeyDown(shift ? '^' : '6', VK_6, mods); break;
        case SC_7: handler->KeyDown(shift ? '&' : '7', VK_7, mods); break;
        case SC_8: handler->KeyDown(shift ? '*' : '8', VK_8, mods); break;
        case SC_9: handler->KeyDown(shift ? '(' : '9', VK_9, mods); break;
        
        // Special
        case SC_BACKTICK: handler->KeyDown(shift ? '~' : '`', VK_BACKTICK, mods); break;
        case SC_MINUS: handler->KeyDown(shift ? '_' : '-', VK_MINUS, mods); break;
        case SC_EQUALS: handler->KeyDown(shift ? '+' : '=', VK_EQUALS, mods); break;
        case SC_BACKSPACE: handler->KeyDown('\b', VK_BACKSPACE, mods); break;

        // Function Keys
        case SC_F1: handler->KeyDown('\0', VK_F1, mods); break;
        case SC_F2: handler->KeyDown('\0', VK_F2, mods); break;
        case SC_F3: handler->KeyDown('\0', VK_F3, mods); break;
        case SC_F4: handler->KeyDown('\0', VK_F4, mods); break;
        case SC_F5: handler->KeyDown('\0', VK_F5, mods); break;
        case SC_F6: handler->KeyDown('\0', VK_F6, mods); break;
        case SC_F7: handler->KeyDown('\0', VK_F7, mods); break;
        case SC_F8: handler->KeyDown('\0', VK_F8, mods); break;
        case SC_F9: handler->KeyDown('\0', VK_F9, mods); break;
        case SC_F10: handler->KeyDown('\0', VK_F10, mods); break;

        // Locks
        case SC_CAPSLOCK: handler->KeyDown('\0', VK_CAPSLOCK, mods); break;
        case SC_NUMLOCK: handler->KeyDown('\0', VK_NUMLOCK, mods); break;
        case SC_SCRLOCK: handler->KeyDown('\0', VK_SCRLOCK, mods); break;

        // Miscellaneous
        case SC_LSHIFT: handler->KeyDown('\0', VK_LSHIFT, mods); break;
        case SC_RSHIFT: handler->KeyDown('\0', VK_RSHIFT, mods); break;
        case SC_LCTRL: handler->KeyDown('\0', VK_LCTRL, mods); break;
        case SC_LALT: handler->KeyDown('\0', VK_LALT, mods); break;
        case SC_ENTER: handler->KeyDown('\n', VK_ENTER, mods); break;
        case SC_SPACE: handler->KeyDown(' ', VK_SPACE, mods); break;

        // Punctuation
        case SC_BRACKET_OPEN: handler->KeyDown(shift ? '{' : '[', VK_BRACKET_OPEN, mods); break;
        case SC_BRACKET_CLOSE: handler->KeyDown(shift ? '}' : ']', VK_BRACKET_CLOSE, mods); break;
        case SC_BACKSLASH: handler->KeyDown(shift ? '|' : '\\', VK_BACKSLASH, mods); break;
        case SC_SEMI_COLON: handler->KeyDown(shift ? ':' : ';', VK_SEMI_COLON, mods); break;
        case SC_SINGLE_QUOTE: handler->KeyDown(shift ? '"' : '\'', VK_SINGLE_QUOTE, mods); break;
        case SC_COMMA: handler->KeyDown(shift ? '<' : ',', VK_COMMA, mods); break;
        case SC_PERIOD: handler->KeyDown(shift ? '>' : '.', VK_PERIOD, mods); break;
        case SC_SLASH: handler->KeyDown(shift ? '?' : '/', VK_SLASH, mods); break;

    }

    return rsp;

}

void KeyboardDriver::Activate(){
    while(command.Read() & 0x1)
        data.Read();

    command.Write(0xae); // activate interrupts (enable keyboard command)
    
    command.Write(0x20); // command 0x20 = read controller command byte

    uint8_t status = (data.Read() | 1) & ~0x10; // set controller command bit 1 and disable bit 4
    
    command.Write(0x60); // command 0x60 = set controller command byte
    
    data.Write(status); // write the command byte
    data.Write(0xf4); // not sure what this does
}