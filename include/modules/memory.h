#pragma once

#include "common/types.h"

#define MIN_CHUNK_SIZE 0



class MemoryManager {

    private:
        // Memory chunk struct inserted before every memory chunk
        // Two unallocated memory chunks one next to another should
        // never occur, as they should be merged when deallocated
        struct MemoryChunk {
            MemoryChunk * prev;
            MemoryChunk * next;
            uint8_t allocated;
            size_t size;
        }__attribute__((packed));
        
        // Pointer to first memory chunk in heap
        MemoryChunk * first;

        // Merge the target chunk with the one to its left
        inline void MergeLeft(MemoryChunk * toDelete);

    public:
        // The active memory manager, used for memory allocation 
        // without needing a reference to the manager
        static MemoryManager * activeMemoryManager;

        MemoryManager(void * heapStart, void * heapEnd);
        ~MemoryManager();

        void * malloc(size_t size);
        void free(void * ptr);

};

inline void * malloc(size_t size) {
    if(MemoryManager::activeMemoryManager != nullptr)
        return MemoryManager::activeMemoryManager->malloc(size);
    return nullptr;
}

inline void free(void * ptr) {
    if(MemoryManager::activeMemoryManager != nullptr)
        return MemoryManager::activeMemoryManager->free(ptr);
}

// void * operator new(unsigned size);
// void * operator new[](unsigned size);

// void operator delete(void * ptr);
// void operator delete[](void * ptr);

void memcpy(const void * src, void * dest, size_t size);
void memcpy_f(const void * src, void * dest, size_t size);

void memset(void * dest, size_t size, uint8_t value);
void memset_f(void * dest, size_t size, uint8_t value);