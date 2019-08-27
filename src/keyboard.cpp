#include "keyboard.h"
#include "lib.h"

KeyboardEventHander::KeyboardEventHander(){}
KeyboardEventHander::~KeyboardEventHander(){}

void KeyboardEventHander::KeyDown(char){}
void KeyboardEventHander::KeyUp(char){}

KeyboardDriver::KeyboardDriver(KeyboardEventHander * handler): 
    data(KEYBOARD_DATA_PORT),
    command(KEYBOARD_COMMAND_PORT) 
{
    this->handler = handler;
}

KeyboardDriver::~KeyboardDriver(){}

uint64_t KeyboardDriver::HandleInterrupt(uint64_t rsp){

    // Read the key that was pressed.
    uint8_t key = data.Read();

    // Some debug output.
    printf("Keyboard - "); 
    printh(key);
    printf();

    // If the handler is present, call it.
    if(handler != NULL)

        // switch case here for character keycodes
        handler->KeyDown(key);

    return rsp;

}

void KeyboardDriver::Activate(){
    while(command.Read() & 0x1)
        data.Read();

    command.Write(0xae); // activate interrupts (enable keyboard command)
    
    // This line and after cause the interrupt and the exception.
    command.Write(0x20); // command 0x20 = read controller command byte

    uint8_t status = (data.Read() | 1) & ~0x10; // set controller command bit 1 and disable bit 4
    
    command.Write(0x60); // command 0x60 = set controller command byte
    
    data.Write(status); // write the command byte
    data.Write(0xf4); // not sure what this does
}