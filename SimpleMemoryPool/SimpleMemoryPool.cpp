#include "SimpleMemoryPool.h"
#include <algorithm>
#include <cstdio>
#include <vector>

struct SimpleMemoryPool::Impl
{
    unsigned long long           totalSize;
    size_t                       used = 0;
    size_t                       chunkSize;
    std::vector<unsigned char *> freePtrList;
    std::vector<void *>          usingPtrList;
    void *                       start = nullptr;

    Impl(const unsigned long long totalSize, const size_t chunkSize);
    ~Impl();
};

SimpleMemoryPool::Impl::Impl(const unsigned long long totalSize, const size_t chunkSize) 
            : totalSize(totalSize), chunkSize(chunkSize) {
    if (nullptr != start) {
        free(start);
    }
    start = malloc(totalSize);
    if (nullptr == start) {
        printf("COULD NOT ALLOCATE %llu memory\n", totalSize);
    }
    else {
        printf("Allocation Successfull\n");
    }
    auto nChunks = totalSize / chunkSize;
    freePtrList.reserve(nChunks);
    for (int i = 0; i < nChunks; ++i) {
        unsigned char* address = (unsigned char*)start + i * chunkSize;
        freePtrList.push_back(address);
    }
}

SimpleMemoryPool::Impl::~Impl() {
    free(start);
    start = nullptr;
}

SimpleMemoryPool::SimpleMemoryPool(const unsigned long long totalSize, const size_t chunkSize)
    : m_pimpl(new Impl(totalSize, chunkSize)) {
}

SimpleMemoryPool::~SimpleMemoryPool() {

}

void * SimpleMemoryPool::allocateMem() {
    void * ret = nullptr;
    if (!m_pimpl->freePtrList.empty()) {
        ret = m_pimpl->freePtrList.back();
        m_pimpl->usingPtrList.push_back(ret);
        m_pimpl->freePtrList.pop_back();
        m_pimpl->used += m_pimpl->chunkSize;
    }
    return ret;
}

void SimpleMemoryPool::freeMem(void * ptr) {
    m_pimpl->used -= m_pimpl->chunkSize;
    auto it = std::find(m_pimpl->usingPtrList.begin(), m_pimpl->usingPtrList.end(), ptr);
    if (it != m_pimpl->usingPtrList.end()) {
        m_pimpl->usingPtrList.erase(it);
    }
    m_pimpl->freePtrList.push_back((unsigned char*)ptr);
}

void logMem(const SimpleMemoryPool * mem) {
    printf("================\n");
    printf("Total Mem : %llu, used Mem : %llu\n", mem->m_pimpl->totalSize, mem->m_pimpl->used);
    auto totalChunks = mem->m_pimpl->totalSize / mem->m_pimpl->chunkSize;
    printf("Total chuncks : %llu, used chuncks : %llu\n", totalChunks, totalChunks - mem->m_pimpl->freePtrList.size());
}
