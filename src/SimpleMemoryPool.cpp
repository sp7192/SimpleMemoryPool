#include "SimpleMemoryPool.h"

#include <algorithm>
#include <cstdio>
#include <vector>

struct SimpleMemoryPool::Impl
{
    unsigned long long           memoryTotalSize;
    size_t                       memoryUsedSize;
    size_t                       memoryBlockSize;
    std::vector<MemoryBlock>     freeMemoryBlocks;
    std::vector<MemoryBlock>     usedMemoryBlocks;
    void *                       memoryBlockStartPtr;

    Impl(const unsigned long long totalSize, const size_t chunkSize);
    ~Impl();
};

SimpleMemoryPool::Impl::Impl(const unsigned long long totalSize, const size_t chunkSize) 
            : memoryTotalSize(totalSize), memoryUsedSize(0), memoryBlockSize(chunkSize), memoryBlockStartPtr(nullptr) {
    memoryBlockStartPtr = malloc(totalSize);
    if (!memoryBlockStartPtr) {
        printf("COULD NOT ALLOCATE %llu memory\n", totalSize);
    }
    else {
        printf("Allocation Successfull\n");
    }
    auto nChunks = totalSize / chunkSize;
    freeMemoryBlocks.reserve(nChunks);
    for (int i = 0; i < nChunks; ++i) {
        unsigned char* address = (unsigned char*)memoryBlockStartPtr + i * chunkSize;
        freeMemoryBlocks.push_back({address, chunkSize});
    }
}

SimpleMemoryPool::Impl::~Impl() {
    if (memoryBlockStartPtr) {
        free(memoryBlockStartPtr);
        memoryBlockStartPtr = nullptr;
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
    if (!m_pimpl->freeMemoryBlocks.empty()) {
        ret = m_pimpl->freeMemoryBlocks.back();
        m_pimpl->usedMemoryBlocks.push_back(ret);
        m_pimpl->freeMemoryBlocks.pop_back();
        m_pimpl->memoryUsedSize += m_pimpl->memoryBlockSize;
    }
    return ret;
}

bool SimpleMemoryPool::freeMem(void * ptr) {
    bool ret = false;
    if (ptr) {
        auto it = std::find_if(m_pimpl->usedMemoryBlocks.cbegin(), m_pimpl->usedMemoryBlocks.cend(),
            [=](const MemoryBlock & m1) {
                return  m1.ptr == ptr && m1.size == m_pimpl->memoryBlockSize;
            });

        if (it != m_pimpl->usedMemoryBlocks.end()) {
            m_pimpl->memoryUsedSize -= m_pimpl->memoryBlockSize;
            m_pimpl->usedMemoryBlocks.erase(it);
            m_pimpl->freeMemoryBlocks.push_back({ (unsigned char*)ptr, m_pimpl->memoryBlockSize });
            ret = true;
        }
    }
    return ret;
}

unsigned long long  SimpleMemoryPool::getMemoryTotalSize() const {
    return m_pimpl->memoryTotalSize;
}

size_t SimpleMemoryPool::getMemoryUsedSize() const {
    return m_pimpl->memoryUsedSize;
}

size_t SimpleMemoryPool::getMemoryBlockSize() const {
    return m_pimpl->memoryBlockSize;
}

size_t SimpleMemoryPool::getMemoryBlocksCount() const {
    return m_pimpl->memoryTotalSize / m_pimpl->memoryBlockSize;
}

size_t SimpleMemoryPool::getFreeMemoryBlocksCount() const {
    return m_pimpl->freeMemoryBlocks.size();
}

size_t SimpleMemoryPool::getUsedMemoryBlocksCount() const {
    return m_pimpl->usedMemoryBlocks.size();
}


void logMem(const SimpleMemoryPool * mem) {
    if (mem) {
        printf("================\n");
        printf("Total Memory size : %llu, memoryUsedSize Mem : %llu\n", mem->getMemoryTotalSize(), mem->getMemoryUsedSize());
        printf("Total chuncks : %llu, memoryUsedSize chuncks : %llu\n", mem->getMemoryBlocksCount(),
                                                                        mem->getMemoryBlocksCount() - mem->getFreeMemoryBlocksCount());
    }
}
