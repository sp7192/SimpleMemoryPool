#include "SimpleMemoryPool.h"

#include <algorithm>
#include <cstdio>
#include <vector>

struct SimpleMemoryPool::Impl
{
    unsigned long long           totalSize;
    size_t                       used;
    size_t                       chunkSize;
    std::vector<MemoryBlock>     freePtrList;
    std::vector<MemoryBlock>     usingPtrList;
    void *                       start;

    Impl(const unsigned long long totalSize, const size_t chunkSize);
    ~Impl();
};

SimpleMemoryPool::Impl::Impl(const unsigned long long totalSize, const size_t chunkSize) 
            : totalSize(totalSize), chunkSize(chunkSize) {
    if (nullptr != start) {
        free(start);
    }
    start = malloc(totalSize);
    if (!start) {
        printf("COULD NOT ALLOCATE %llu memory\n", totalSize);
    }
    else {
        printf("Allocation Successfull\n");
    }
    auto nChunks = totalSize / chunkSize;
    freePtrList.reserve(nChunks);
    for (int i = 0; i < nChunks; ++i) {
        unsigned char* address = (unsigned char*)start + i * chunkSize;
        freePtrList.push_back({address, chunkSize});
    }
}

SimpleMemoryPool::Impl::~Impl() {
    if (start) {
        free(start);
        start = nullptr;
    }
}

SimpleMemoryPool::SimpleMemoryPool(const unsigned long long totalSize, const size_t chunkSize)
    : m_pimpl(new Impl(totalSize, chunkSize)) {
}

SimpleMemoryPool::~SimpleMemoryPool() {
    delete m_pimpl;
}

MemoryBlock SimpleMemoryPool::allocateMem() {
    MemoryBlock ret;
    if (!m_pimpl->freePtrList.empty()) {
        ret = m_pimpl->freePtrList.back();
        m_pimpl->usingPtrList.push_back(ret);
        m_pimpl->freePtrList.pop_back();
        m_pimpl->used += m_pimpl->chunkSize;
    }
    return ret;
}

bool SimpleMemoryPool::freeMem(void * ptr) {
    bool ret = false;
    if (ptr) {
        auto it = std::find_if(m_pimpl->usingPtrList.cbegin(), m_pimpl->usingPtrList.cend(),
            [=](const MemoryBlock & m1) {
                return  m1.ptr == ptr && m1.size == m_pimpl->chunkSize;
            });

        if (it != m_pimpl->usingPtrList.end()) {
            m_pimpl->used -= m_pimpl->chunkSize;
            m_pimpl->usingPtrList.erase(it);
            m_pimpl->freePtrList.push_back({ (unsigned char*)ptr, m_pimpl->chunkSize });
            ret = true;
        }
    }
    return ret;
}

void logMem(const SimpleMemoryPool * mem) {
    if (mem) {
        printf("================\n");
        printf("Total Mem : %llu, used Mem : %llu\n", mem->m_pimpl->totalSize, mem->m_pimpl->used);
        auto totalChunks = mem->m_pimpl->totalSize / mem->m_pimpl->chunkSize;
        printf("Total chuncks : %llu, used chuncks : %llu\n", totalChunks, totalChunks - mem->m_pimpl->freePtrList.size());
    }
}
