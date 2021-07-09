#pragma once

namespace SimpleMemoryPool {
    struct MemoryBlock
    {
        unsigned char*  ptr;
        size_t          size;

        MemoryBlock() : ptr(nullptr), size(0) {}
        MemoryBlock(unsigned char * _ptr, size_t _size) : ptr(_ptr), size(_size) {}
    };
}
