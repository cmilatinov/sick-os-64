#include "drivers/mouse.h"

MouseDriver::MouseDriver(): 
    data(MOUSE_DATA_PORT),
    command(MOUSE_COMMAND_PORT) 
{}

MouseDriver::~MouseDriver() {}

uint64_t MouseDriver::HandleInterrupt(uint64_t rsp) {

    // Check if data is available to be read
    uint8_t status = command.Read();
    if(!(status & 0x20))
        return rsp;

    // Read data
    buffer[offset] = data.Read();

    // Increment offset
    offset = (offset + 1) % MOUSE_BUFFER_SIZE;

    // If no handlers or we have not read all 3 bytes, return
    if((mouseButtonDown == nullptr && mouseButtonUp == nullptr && mouseMove == nullptr) || offset == 0)
        return rsp;
    
    // Mouse movement check
    if((buffer[MOUSE_OFFSET_DX] || buffer[MOUSE_OFFSET_DY]) && mouseMove) {
        uint32_t dx =   buffer[MOUSE_OFFSET_INFO] & MOUSE_SIGN_DX ? 
                        0xFFFFFF00 | buffer[MOUSE_OFFSET_DX] :
                        0x0 | buffer[MOUSE_OFFSET_DX];
        uint32_t dy =   buffer[MOUSE_OFFSET_INFO] & MOUSE_SIGN_DY ? 
                        0xFFFFFF00 | buffer[MOUSE_OFFSET_DY] :
                        0x0 | buffer[MOUSE_OFFSET_DY];
        mouseMove(dx, dy);
    }

    // Mouse buttons check
    for(uint8_t i = 0; i < 3; i++) {

        uint8_t current = buttons & (1 << i);
        if((buffer[MOUSE_OFFSET_INFO] & (1 << i)) != current) {
            
            // Mouse button released
            if(current && mouseButtonUp)
                mouseButtonUp(1 << i);

            // Mouse button pressed
            else if(!current && mouseButtonDown)
                mouseButtonDown(1 << i);

        }

    }
    
    // Save button state
    buttons = buffer[MOUSE_OFFSET_INFO] & 0b111;

    return rsp;
}

void MouseDriver::Activate() {
    
    // Controller Command 0xA8 = Enable mouse port
    command.Write(0xA8);
    
    // Controller Command 0x20 = Read controller command byte
    command.Write(0x20);
    
    // Set controller command bit 1
    // Bit 1 = Mouse interrupt enable
    uint8_t status = data.Read() | 2;
    
    // Controller Command 0x60 = Write controller command byte
    command.Write(0x60);

    // Write the command byte to data port
    data.Write(status);
    
    // Controller Command 0xD4 = Write to mouse
    command.Write(0xD4);

    // Mouse Command 0xF4 = Enable packet streaming
    // Mouse starts sending automatic packets when it moves or is clicked.
    data.Write(0xF4);

    // Read response byte to clear it
    data.Read();

}

void MouseDriver::OnMouseButtonDown(void (*mouseButtonDown) (uint8_t button)) {
    this->mouseButtonDown = mouseButtonDown;
}

void MouseDriver::OnMouseButtonUp(void (*mouseButtonUp) (uint8_t button)) {
    this->mouseButtonUp = mouseButtonUp;
}

void MouseDriver::OnMouseMove(void (*mouseMove) (uint32_t dx, uint32_t dy)) {
    this->mouseMove = mouseMove;
}