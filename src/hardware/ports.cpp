#include "hardware/ports.h"
#include "modules/lib.h"

// Constructors
Port8S::Port8S(uint16_t port) : 
    port(port) {}
Port8::Port8(uint16_t port) : 
    port(port) {}
Port16::Port16(uint16_t port) : 
    port(port) {}
Port32::Port32(uint16_t port) : 
    port(port) {}

uint8_t Port8S::Read() const {
    return Read(port);
}

void Port8S::Write(uint8_t data) const {
    Write(port, data);
}

uint8_t Port8::Read() const {
    return Read(port);
}

void Port8::Write(uint8_t data) const {
    Write(port, data);
}

uint16_t Port16::Read() const {
    return Read(port);
}

void Port16::Write(uint16_t data) const {
    Write(port, data);
}

uint32_t Port32::Read() const {
    return Read(port);
}

void Port32::Write(uint32_t data) const {
    Write(port, data);
}