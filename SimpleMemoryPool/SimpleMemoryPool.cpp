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

    Impl(const unsigned long long totalSize, const size_t chunkSize)
        : totalSize(totalSize), chunkSize(chunkSize) {
    }

    ~Impl() {
        free(start);
        start = nullptr;
    }
};

SimpleMemoryPool::SimpleMemoryPool(const unsigned long long totalSize, const size_t chunkSize)
    : m_pimpl(new Impl(totalSize, chunkSize)) {
}

SimpleMemoryPool::~SimpleMemoryPool() {

}

void SimpleMemoryPool::init() {
    if (nullptr != m_pimpl->start) {
        free(m_pimpl->start);
    }
    m_pimpl->start = malloc(m_pimpl->totalSize);
    if (nullptr == m_pimpl->start) {
        printf("COULD NOT ALLOCATE %llu memory\n", m_pimpl->totalSize);
    }
    else {
        printf("Allocation Successfull\n");
    }
    reset();
}

void* SimpleMemoryPool::allocateMem() {
    void* ret = nullptr;
    if (!m_pimpl->freePtrList.empty()) {
        ret = m_pimpl->freePtrList.back();
        m_pimpl->usingPtrList.push_back(ret);
        m_pimpl->freePtrList.pop_back();
        m_pimpl->used += m_pimpl->chunkSize;
    }
    return ret;
}

void SimpleMemoryPool::freeMem(void* ptr) {
    m_pimpl->used -= m_pimpl->chunkSize;
    auto it = std::find(m_pimpl->usingPtrList.begin(), m_pimpl->usingPtrList.end(), ptr);
    if (it != m_pimpl->usingPtrList.end()) {
        m_pimpl->usingPtrList.erase(it);
    }
    m_pimpl->freePtrList.push_back((unsigned char*)ptr);
}

void SimpleMemoryPool::reset() {
    m_pimpl->used = 0;
    m_pimpl->usingPtrList.clear();
    m_pimpl->freePtrList.clear();
    auto nChunks = m_pimpl->totalSize / m_pimpl->chunkSize;
    m_pimpl->freePtrList.reserve(nChunks);
    for (int i = 0; i < nChunks; ++i) {
        unsigned char* address = (unsigned char*)m_pimpl->start + i * m_pimpl->chunkSize;
        m_pimpl->freePtrList.push_back(address);
    }
}

#if 0
std::vector<void*> SimpleMemoryPool::getUsingMemories() const {
    return m_pimpl->usingPtrList;
}
#endif
void logMem(const SimpleMemoryPool * mem) {
    printf("================\n");
    printf("Total Mem : %llu, used Mem : %llu\n", mem->m_pimpl->totalSize, mem->m_pimpl->used);
    printf("Total chuncks : %llu, used chuncks : %llu\n", mem->m_pimpl->totalSize / mem->m_pimpl->chunkSize, mem->m_pimpl->freePtrList.size());
}
