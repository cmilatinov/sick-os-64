#pragma once
    
typedef unsigned char BYTE;

typedef char int8_t;
typedef unsigned char uint8_t;

typedef short int16_t;
typedef unsigned short uint16_t;

typedef int int32_t;
typedef unsigned int uint32_t;

typedef long long int int64_t;
typedef unsigned long long int uint64_t;

typedef uint64_t size_t;

#define NULL 0

#define UINT8_MAX   0xFF
#define UINT16_MAX  0xFFFF
#define UINT32_MAX  0xFFFFFFFF
#define UINT64_MAX  0xFFFFFFFFFFFFFFFF

// GCC Compiler Directive - Pack the struct/class so the compiler doesn't add useless padding bytes.
#define PACKED __attribute__((packed))

// Perform an atomic exchange of data between a register and memory
#define Atomic_Exchange(mem, reg) \
   asm volatile ("xchgl %0, %1" : "+r" (reg), "+m" (mem) :)

// Perform an atomic OR of data between a register and memory
#define Atomic_Or(mem, reg) \
   asm volatile ("lock orl %1, %0" :"+m" (mem) :"r" (reg))

// Min / Max macros
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))