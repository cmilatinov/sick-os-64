#include "ports.h"
#include "interrupts.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_COMMAND_PORT 0x64

// Keyboard Handler Class
class KeyboardEventHander {

    public:
    KeyboardEventHander();
    ~KeyboardEventHander();

    virtual void KeyDown(char);
    virtual void KeyUp(char);

};

// Keyboard Driver Class
class KeyboardDriver : public InterruptHandler{

    private:
    Port8 data;
    Port8 command;

    KeyboardEventHander * handler;

    public:
    KeyboardDriver(KeyboardEventHander * handler);
    ~KeyboardDriver();

    virtual uint64_t HandleInterrupt(uint64_t rsp);
    void Activate();

};