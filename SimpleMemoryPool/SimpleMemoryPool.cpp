#include "SimpleMemoryPool.h"
#include <algorithm>
#include <cstdio>

SimpleMemoryPool::SimpleMemoryPool(const unsigned long long sz, const size_t chSz)
    : m_totalSize(sz), m_chunkSize(chSz), m_start(nullptr) {
}

SimpleMemoryPool::~SimpleMemoryPool() {
    free(m_start);
    m_start = nullptr;
}

void SimpleMemoryPool::init() {
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

void* SimpleMemoryPool::allocateMem() {
    void* ret = nullptr;
    if (!m_freePtrList.empty()) {
        ret = m_freePtrList.back();
        m_usingPtrList.push_back(ret);
        m_freePtrList.pop_back();
        m_used += m_chunkSize;
    }
    return ret;
}

void SimpleMemoryPool::freeMem(void* ptr) {
    m_used -= m_chunkSize;
    auto it = std::find(m_usingPtrList.begin(), m_usingPtrList.end(), ptr);
    if (it != m_usingPtrList.end()) {
        m_usingPtrList.erase(it);
    }
    m_freePtrList.push_back((unsigned char*)ptr);
}

void SimpleMemoryPool::reset() {
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

std::vector<void*> SimpleMemoryPool::getUsingMemories() const {
    return m_usingPtrList;
}

void logMem(const SimpleMemoryPool * mem) {
    printf("================\n");
    printf("Total Mem : %llu, used Mem : %llu\n", mem->m_totalSize, mem->m_used);
    printf("Total chuncks : %llu, used chuncks : %llu\n", mem->m_totalSize / mem->m_chunkSize, mem->m_freePtrList.size());
}
