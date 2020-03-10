#include "hardware/ports.h"
#include "hardware/interrupts.h"
#include "utils/keys.h"

#define MOUSE_DATA_PORT     0x60
#define MOUSE_COMMAND_PORT  0x64

#define MOUSE_BUFFER_SIZE 3

#define MOUSE_OFFSET_INFO   0
#define MOUSE_OFFSET_DX     1
#define MOUSE_OFFSET_DY     2

#define MOUSE_SIGN_DX   0b00010000
#define MOUSE_SIGN_DY   0b00100000

#define MOUSE_BTN_MIDDLE    0b00000100
#define MOUSE_BTN_RIGHT     0b00000010
#define MOUSE_BTN_LEFT      0b00000001

// Keyboard Driver Class
class MouseDriver : public InterruptHandler {

    private:
    Port8 data;
    Port8 command;

    uint8_t buffer[MOUSE_BUFFER_SIZE];
    uint8_t offset = 0;

    uint8_t buttons = 0;

    void (*mouseButtonDown) (uint8_t button);
    void (*mouseButtonUp) (uint8_t button);
    void (*mouseMove) (uint32_t dx, uint32_t dy);

    public:
    MouseDriver();
    ~MouseDriver();

    virtual uint64_t HandleInterrupt(uint64_t rsp);
    void Activate();

    void OnMouseButtonDown(void (*mouseButtonDown) (uint8_t button));
    void OnMouseButtonUp(void (*mouseButtonUp) (uint8_t button));
    void OnMouseMove(void (*mouseMove) (uint32_t dx, uint32_t dy));

};