#include "hardware/ports.h"
#include "hardware/interrupts.h"
#include "utils/keys.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_COMMAND_PORT 0x64

// Keyboard Driver Class
class KeyboardDriver : public InterruptHandler {

    private:
    Port8 data;
    Port8 command;

    void (*keyUp)(char, uint8_t, uint8_t) = nullptr;
    void (*keyDown)(char, uint8_t, uint8_t) = nullptr;

    public:
    KeyboardDriver();
    ~KeyboardDriver();

    virtual uint64_t HandleInterrupt(uint64_t rsp);
    void Activate();

    void OnKeyDown(void (*keyDown)(char, uint8_t, uint8_t));
    void OnKeyUp(void (*keyUp)(char, uint8_t, uint8_t));
    
};