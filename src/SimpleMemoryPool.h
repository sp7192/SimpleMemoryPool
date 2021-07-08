#pragma once

#include <utility>
#include <new>

#include "MemoryBlock.h"

class SimpleMemoryPool
{
    class Impl;
    Impl * m_pimpl;

public:
    SimpleMemoryPool(const unsigned long long totalSize, const size_t chunckSize);
    ~SimpleMemoryPool();

    SimpleMemoryPool(const SimpleMemoryPool &)              = delete;
    SimpleMemoryPool & operator=(const SimpleMemoryPool &)  = delete;
    SimpleMemoryPool(const SimpleMemoryPool &&)             = delete;
    SimpleMemoryPool& operator=(const SimpleMemoryPool &&)  = delete;

    MemoryBlock  allocateMem();
    bool freeMem(void * ptr);

    friend void logMem(const SimpleMemoryPool * mem);

    template<typename T, class ... Args>
    T * construct(Args && ... args);
    template<typename T>
    void destruct(T * ptr);

    unsigned long long  getMemoryTotalSize() const;
    size_t getMemoryUsedSize() const;
    size_t getMemoryBlockSize() const;
    size_t getMemoryBlocksCount() const;
    size_t getFreeMemoryBlocksCount() const;
    size_t getUsedMemoryBlocksCount() const;
};
template<typename T, class ... Args>
T * SimpleMemoryPool::construct(Args && ... args) {
    T * ret = nullptr;
    MemoryBlock mem = allocateMem();
    if (mem.ptr && mem.size >= sizeof(T)) {
        ret = new (mem.ptr) T(std::forward<Args>(args)...);
    }
    return ret;
}

template<typename T>
void SimpleMemoryPool::destruct(T * ptr) {
    if (ptr) {
        ptr->~T();
        freeMem((void*)ptr);
    }
}