#pragma once

#include <utility>
#include <new>

#define ALLOCATOR_SIZE 1024 * 1024
#define ALLOCATOR_CHUNCK_SIZE 256


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

    void    init();
    void *  allocateMem();
    void    freeMem(void * ptr);
    void    reset();

    friend void logMem(const SimpleMemoryPool * mem);

    template<typename T, class ... Args>
    T * construct(Args && ... args);
    template<typename T>
    void destruct(T* ptr);
};
template<typename T, class ... Args>
T * SimpleMemoryPool::construct(Args && ... args) {
    void * p = allocateMem();
    T * ptr = new (p) T(std::forward<Args>(args)...);
    return ptr;
}

template<typename T>
void SimpleMemoryPool::destruct(T* ptr) {
    ptr->~T();
    freeMem((void*)ptr);
}