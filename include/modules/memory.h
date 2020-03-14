#pragma once

#include "common/types.h"

struct MemoryChunk {
    MemoryChunk * prev;
    MemoryChunk * next;
    uint8_t allocated;
    uint32_t size;
}__attribute__((packed));