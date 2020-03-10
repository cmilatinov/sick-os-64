#include "hardware/ports.h"

// Constructors
Port8S::Port8S(uint16_t port){
    this->port = port;
}
Port8::Port8(uint16_t port){
    this->port = port;
}
Port16::Port16(uint16_t port){
    this->port = port;
}
Port32::Port32(uint16_t port){
    this->port = port;
}

// Destructors
Port8S::~Port8S(){}
Port8::~Port8(){}
Port16::~Port16(){}
Port32::~Port32(){}

uint8_t Port8S::Read(){
    return Read(port);
}

void Port8S::Write(uint8_t data){
    Write(port, data);
}

uint8_t Port8::Read(){
    return Read(port);
}

void Port8::Write(uint8_t data){
    Write(port, data);
}

uint16_t Port16::Read(){
    return Read(port);
}

void Port16::Write(uint16_t data){
    Write(port, data);
}

uint32_t Port32::Read(){
    return Read(port);
}

void Port32::Write(uint32_t data){
    Write(port, data);
}