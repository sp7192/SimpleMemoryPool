#pragma once

namespace SimpleMemoryPool
{
    struct MemoryBlock
    {
        unsigned char * ptr;
        size_t          size;

        MemoryBlock() : ptr(nullptr), size(0) {}
        MemoryBlock(unsigned char * _ptr, size_t _size) : ptr(_ptr), size(_size) {}
    };

    template <typename T>
    struct ArrayBlock
    {
        T * ptr;
        size_t count;

        ArrayBlock() : ptr(nullptr), count(0) {}
        ArrayBlock(T * _ptr, size_t _size) : ptr(_ptr), size(_size) {}

        T & operator [] (size_t index)
        {
            return ptr[index];
        }
    };
}
