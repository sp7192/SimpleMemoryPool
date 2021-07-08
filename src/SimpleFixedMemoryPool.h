#pragma once

#include <utility>
#include <new>

#include "MemoryBlock.h"

class SimpleFixedMemoryPool
{
    struct Impl;
    Impl * m_pimpl;

public:
    SimpleFixedMemoryPool(const unsigned long long totalSize, const size_t chunckSize);
    ~SimpleFixedMemoryPool();

    SimpleFixedMemoryPool(const SimpleFixedMemoryPool &)              = delete;
    SimpleFixedMemoryPool & operator=(const SimpleFixedMemoryPool &)  = delete;
    SimpleFixedMemoryPool(const SimpleFixedMemoryPool &&)             = delete;
    SimpleFixedMemoryPool& operator=(const SimpleFixedMemoryPool &&)  = delete;

    MemoryBlock  allocateMem();
    bool freeMem(void * ptr);

    friend void logMem(const SimpleFixedMemoryPool * mem);

    template<typename T, class ... Args>
    T * construct(Args && ... args);
    template<typename T>
    bool destruct(T * ptr);

    unsigned long long  getMemoryTotalSize() const;
    size_t getMemoryUsedSize() const;
    size_t getMemoryBlockSize() const;
    size_t getMemoryBlocksCount() const;
    size_t getFreeMemoryBlocksCount() const;
    size_t getUsedMemoryBlocksCount() const;
};
template<typename T, class ... Args>
T * SimpleFixedMemoryPool::construct(Args && ... args) {
    T * ret = nullptr;
    MemoryBlock mem = allocateMem();
    if (mem.ptr && mem.size >= sizeof(T)) {
        ret = new (mem.ptr) T(std::forward<Args>(args)...);
    }
    return ret;
}

template<typename T>
bool SimpleFixedMemoryPool::destruct(T * ptr) {
    bool ret = false;
    if (ptr) {
        ptr->~T();
        ret = freeMem((void*)ptr);
    }
    return ret;
}