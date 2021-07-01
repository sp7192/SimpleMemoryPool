#pragma once

#include <cstdio>
#include <vector>
#include <algorithm>

#define ALLOCATOR_SIZE 1024 * 1024
#define ALLOCATOR_CHUNCK_SIZE 256

class SimpleMemoryPool
{
    unsigned long long m_totalSize;
    size_t m_used = 0;
    size_t m_chunkSize;
    std::vector<unsigned char*> m_freePtrList;
    std::vector<void*> m_usingPtrList;
    void* m_start;

public:
    SimpleMemoryPool(const unsigned long long sz, const size_t chSz)
        : m_totalSize(sz), m_chunkSize(chSz), m_start(nullptr)
    {}

    ~SimpleMemoryPool() {
        free(m_start);
        m_start = nullptr;
    }

    void init() {
        if (nullptr != m_start) {
            free(m_start);
        }
        m_start = malloc(m_totalSize);
        if (nullptr == m_start) {
            printf("COULD NOT ALLOCATE %llu memory\n", m_totalSize);
        }
        else {
            printf("Allocation Successfull\n");
        }
        reset();
    }

    void* allocateMem() {
        void * ret = nullptr;
        if (!m_freePtrList.empty()) {
            ret = m_freePtrList.back();
            m_usingPtrList.push_back(ret);
            m_freePtrList.pop_back();
            m_used += m_chunkSize;
        }
        return ret;
    }


    void freeMem(void* ptr)
    {
        m_used -= m_chunkSize;
        auto it = std::find(m_usingPtrList.begin(), m_usingPtrList.end(), ptr);
        if (it != m_usingPtrList.end()) {
            m_usingPtrList.erase(it);
        }
        m_freePtrList.push_back((unsigned char*)ptr);
    }

    void reset()
    {
        m_used = 0;
        m_usingPtrList.clear();
        m_freePtrList.clear();
        auto nChunks = m_totalSize / m_chunkSize;
        m_freePtrList.reserve(nChunks);
        for (int i = 0; i < nChunks; ++i) {
            unsigned char* address = (unsigned char*)m_start + i * m_chunkSize;
            m_freePtrList.push_back(address);
        }
    }

    template<typename T, class ... Args>
    T* construct(Args && ... args) {
        void* p = allocateMem();
        T* ptr = new (p) T(std::forward<Args>(args)...);
        return ptr;
    }

    template<typename T>
    void destruct(T* ptr) {
        ptr->~T();
        freeMem((void*)ptr);
    }

    std::vector<void*> getUsingMem() const
    {
        return m_usingPtrList;
    }

    void logMem() const {
        printf("================\n");
        printf("Total Mem : %llu, used Mem : %llu\n", m_totalSize, m_used);
        printf("Total chuncks : %llu, used chuncks : %llu\n", m_totalSize / m_chunkSize, m_freePtrList.size());
    }
};
