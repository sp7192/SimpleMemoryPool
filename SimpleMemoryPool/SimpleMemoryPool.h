#pragma once

#include <vector>

#define ALLOCATOR_SIZE 1024 * 1024
#define ALLOCATOR_CHUNCK_SIZE 256

class SimpleMemoryPool
{
    unsigned long long m_totalSize;
    size_t m_used = 0;
    size_t m_chunkSize;
    std::vector<unsigned char*> m_freePtrList;
    std::vector<void*> m_usingPtrList;
    void * m_start;

public:
    SimpleMemoryPool(const unsigned long long sz, const size_t chSz);
    ~SimpleMemoryPool();

    void    init();
    void *  allocateMem();
    void    freeMem(void * ptr);
    void    reset();

    std::vector<void *> getUsingMemories() const;
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