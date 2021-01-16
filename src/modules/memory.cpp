#include "modules/memory.h"
#include "modules/lib.h"

MemoryManager * MemoryManager::activeMemoryManager = nullptr;

MemoryManager::MemoryManager(void * heapStart, uint32_t heapSize) {
    
    // Create initial chunk
    first = reinterpret_cast<MemoryChunk*>(heapStart);
    first->prev = nullptr;
    first->next = nullptr;
    first->allocated = false;
    first->size = heapSize - sizeof(MemoryChunk);
    
    // Set this manager as the active manager
    activeMemoryManager = this;

}

MemoryManager::~MemoryManager() {
    if(activeMemoryManager == this)
        activeMemoryManager = nullptr;
}

void * MemoryManager::malloc(size_t size) {

    // Can't create empty memory chunks
    if(!size)
        return nullptr;

    MemoryChunk * mem = first;
    do {
        
        // Memory chunk is big enough and not allocated
        if (mem->size >= size && !mem->allocated) {

            // Memory chunk is not big enough to be split in two
            // so just allocate the full chunk
            if (mem->size <= size + sizeof(MemoryChunk) + MIN_CHUNK_SIZE) {
                
                mem->allocated = true;

            // Split chunk in two and allocate first one
            } else {

                // Pointer to next chunk = this chunk + chunk header size + needed size 
                MemoryChunk * next = reinterpret_cast<MemoryChunk*>(reinterpret_cast<uint8_t*>(mem) + sizeof(MemoryChunk) + size);

                // Insert next chunk
                next->prev = mem;
                next->next = mem->next;
                next->allocated = false;
                next->size = mem->size - size - sizeof(MemoryChunk);

                // Resize this chunk and setup pointer to next chunk
                mem->size = size;
                mem->next = next;
                mem->allocated = true;

            }

            return reinterpret_cast<uint8_t*>(mem) + sizeof(MemoryChunk);
        }

    } while ((mem = mem->next) != nullptr);

    return nullptr;

}

void MemoryManager::free(void * ptr) {

    MemoryChunk * mem = reinterpret_cast<MemoryChunk*>(reinterpret_cast<uint8_t*>(ptr) - sizeof(MemoryChunk));

    // Merge left
    if (mem->prev != nullptr && !mem->prev->allocated)
        MergeLeft(mem);

    // Merge right
    if (mem->next != nullptr && !mem->next->allocated) 
        MergeLeft(mem->next);
        
}

inline void MemoryManager::MergeLeft(MemoryChunk * toDelete) {

    // Update left pointer
    toDelete->prev->next = toDelete->next;

    // Update right pointer if not last chunk in list
    if(toDelete->next != nullptr)
        toDelete->next->prev = toDelete->prev;

    // Add this chunk's full size to the left chunk
    toDelete->prev->size += sizeof(MemoryChunk) + toDelete->size;

}

bool MemoryManager::IsMemoryManagerPresent() {
    return activeMemoryManager != nullptr;
}

void * operator new(long unsigned int size) {
    return malloc(size);
}

void * operator new[](long unsigned int size) {
    return malloc(size);
}

void operator delete(void * ptr) {
    free(ptr);
}

void operator delete(void * ptr, long unsigned int size) {
    free(ptr);
}

void operator delete[](void * ptr) {
    free(ptr);
}

void operator delete[](void * ptr, long unsigned int size) {
    free(ptr);
}

void memcpy(const void * src, void * dest, size_t size) {
    
    uint8_t * destPtr = reinterpret_cast<uint8_t*>(dest);
    const uint8_t * srcPtr = reinterpret_cast<const uint8_t*>(src);
    
    for(size_t i = 0; i < size; i++)
        destPtr[i] = srcPtr[i];
        
}

void memcpy_f(const void * src, void * dest, size_t size) {
    
    uint64_t * destPtr = reinterpret_cast<uint64_t*>(dest);
    const uint64_t * srcPtr = reinterpret_cast<const uint64_t*>(src);
    
    for(size_t i = 0; i < size / sizeof(uint64_t); i++)
        destPtr[i] = srcPtr[i];
    
    if(size & 0b111) {

        uint8_t * d = reinterpret_cast<uint8_t*>(destPtr) + size - (size & 0b111);
        const uint8_t * s = reinterpret_cast<const uint8_t*>(srcPtr) + size - (size & 0b111);

        for(uint8_t i = 0; i < (size & 0b111); i++)
            d[i] = s[i];

    }

}

void * memset(void * dest, uint8_t src, size_t c) {
    void * temp = dest;
    __asm__ volatile (
        "rep stosb"
        : "=D"(dest), "=c"(c)
        : "0"(dest), "a"(src), "1"(c)
        : "memory"
    );
    return temp;
}

void memset_f(void * dest, size_t size, uint8_t value) {

    uint64_t * destPtr = reinterpret_cast<uint64_t*>(dest);
    uint8_t _value64[8] = { value, value, value, value, value, value, value, value };
    uint64_t value64 = *reinterpret_cast<uint64_t*>(_value64);

    size_t end = size / sizeof(uint64_t);
    for(size_t i = 0; i < end; i++)
        destPtr[i] = value64;
    
    size_t leftover = size & 0x7;
    if(leftover) {

        uint8_t * d = reinterpret_cast<uint8_t*>(destPtr + end);

        for(uint8_t i = 0; i < leftover; i++)
            d[i] = value;

    }

}