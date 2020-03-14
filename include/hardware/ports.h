#pragma once

#include "common/types.h"

class Port8S {

    private:
    uint16_t port;

    public:
    Port8S(uint16_t port);
    ~Port8S();

    uint8_t Read();
    void Write(uint8_t data);

    static inline uint8_t Read(uint16_t port){
        uint8_t result;
        asm volatile(
            "inb %1, %0\n"
            "1: jmp 1f\n"
            "1: jmp 1f\n"
            "1:"
            : "=a"(result) 
            : "Nd"(port));
        return result;
    }

    static inline void Write(uint16_t port, uint8_t data){
        asm volatile( 
            "outb %0, %1\n"
            "1: jmp 1f\n"
            "1: jmp 1f\n"
            "1:"
            :
            : "a" (data), "Nd"(port)
        );
    }

};

class Port8 {

    private:
    uint16_t port;

    public:
    Port8(uint16_t port);
    ~Port8();

    uint8_t Read();
    void Write(uint8_t data);

    static inline uint8_t Read(uint16_t port){
        uint8_t result;
        asm volatile(
            "inb %1, %0"
            : "=a"(result) 
            : "Nd"(port));
        return result;
    }

    static inline void Write(uint16_t port, uint8_t data){
        asm volatile( 
            "outb %0, %1"
            :
            : "a" (data), "Nd"(port)
        );
    }

};

class Port16 {

    private:
    uint16_t port;

    public:
    Port16(uint16_t port);
    ~Port16();

    uint16_t Read();
    void Write(uint16_t data);

    static inline uint16_t Read(uint16_t port){
        uint16_t result;
        asm volatile(
            "inw %1, %0"
            : "=a"(result) 
            : "Nd"(port));
        return result;
    }

    static inline void Write(uint16_t port, uint16_t data){
        asm volatile( 
            "outw %0, %1"
            :
            : "a" (data), "Nd"(port)
        );
    }

};

class Port32 {

    private:
    uint16_t port;

    public:
    Port32(uint16_t port);
    ~Port32();

    uint32_t Read();
    void Write(uint32_t data);

    static inline uint32_t Read(uint16_t port){
        uint32_t result;
        asm volatile(
            "inl %1, %k0"
            : "=a"(result) 
            : "Nd"(port));
        return result;
    }

    static inline void Write(uint16_t port, uint16_t data){
        asm volatile( 
            "outl %k0, %1"
            :
            : "a" (data), "Nd"(port)
        );
    }

};