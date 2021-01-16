#pragma once

#include "common/keycodes.h"

#include "hardware/interrupts.h"
#include "hardware/pci.h"
#include "hardware/ports.h"

#include "modules/drivers.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_COMMAND_PORT 0x64

// Keyboard Driver Class
class KeyboardDriver : public Driver, public InterruptHandler {

    private:
    const Port8 data = Port8(KEYBOARD_DATA_PORT);
    const Port8 command = Port8(KEYBOARD_COMMAND_PORT);

    void (*keyUp)(char c, uint8_t key, uint8_t mods) = nullptr;
    void (*keyDown)(char c, uint8_t key, uint8_t mods) = nullptr;

    public:
    virtual const char * DisplayName() final { return "Generic Keyboard Driver"; };

    virtual void Load() final;  
    virtual void Reset() final {}
    virtual void Destroy() final {}

    virtual void HandleInterrupt(uint8_t irq) final;

    void OnKeyDown(void (*keyDown)(char, uint8_t, uint8_t));
    void OnKeyUp(void (*keyUp)(char, uint8_t, uint8_t));
    
};