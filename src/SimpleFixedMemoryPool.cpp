#include "SimpleFixedMemoryPool.h"

#include <algorithm>
#include <cstdio>
#include <vector>

namespace SimpleMemoryPool {
    struct SimpleFixedMemoryPool::Impl
    {
        size_t                      memoryTotalSize;
        size_t                      memoryUsedSize;
        size_t                      memoryBlockSize;
        std::vector<MemoryBlock>    freeMemoryBlocks;
        std::vector<MemoryBlock>    usedMemoryBlocks;
        void *                      memoryBlockStartPtr;

        Impl(size_t totalSize, size_t chunkSize);
        ~Impl();
    };

    SimpleFixedMemoryPool::Impl::Impl(size_t totalSize, size_t blockSize)
        : memoryTotalSize(totalSize), memoryUsedSize(0), memoryBlockSize(blockSize), memoryBlockStartPtr(nullptr) {
        try {
            memoryBlockStartPtr = malloc(totalSize);
        } catch(...) {
            printf("COULD NOT ALLOCATE %zu memory\n", totalSize);
            std::terminate();
        }
        if (memoryBlockSize > memoryTotalSize) {
            memoryBlockSize = memoryTotalSize;
        }
        size_t blockCount = memoryBlockSize >  0 ? memoryTotalSize / memoryBlockSize : 0;
        freeMemoryBlocks.reserve(blockCount);
        for (int i = blockCount -1; i >= 0; --i) {
            unsigned char* address = (unsigned char*)memoryBlockStartPtr + i * memoryBlockSize;
            freeMemoryBlocks.push_back({ address, memoryBlockSize });
        }
    }

    SimpleFixedMemoryPool::Impl::~Impl() {
        if (memoryBlockStartPtr) {
            free(memoryBlockStartPtr);
            memoryBlockStartPtr = nullptr;
        }
    }

    SimpleFixedMemoryPool::SimpleFixedMemoryPool(size_t totalSize, size_t blockSize)
        : m_pimpl(new Impl(totalSize, blockSize)) {
    }

    SimpleFixedMemoryPool::~SimpleFixedMemoryPool() {
        delete m_pimpl;
    }

    MemoryBlock SimpleFixedMemoryPool::allocateMemory() {
        MemoryBlock ret;
        if (!m_pimpl->freeMemoryBlocks.empty()) {
            ret = m_pimpl->freeMemoryBlocks.back();
            m_pimpl->usedMemoryBlocks.push_back(ret);
            m_pimpl->freeMemoryBlocks.pop_back();
            m_pimpl->memoryUsedSize += m_pimpl->memoryBlockSize;
        }
        return ret;
    }

    bool SimpleFixedMemoryPool::freeMemory(MemoryBlock * memoryBlock) {
        bool ret = false;
        if (memoryBlock && memoryBlock->ptr) {
            auto it = std::find_if(m_pimpl->usedMemoryBlocks.cbegin(), m_pimpl->usedMemoryBlocks.cend(),
                [=](const MemoryBlock& m1) {
                    return  m1.ptr == memoryBlock->ptr && m1.size == m_pimpl->memoryBlockSize;
                });

            if (it != m_pimpl->usedMemoryBlocks.end()) {
                m_pimpl->memoryUsedSize -= m_pimpl->memoryBlockSize;
                m_pimpl->usedMemoryBlocks.erase(it);
                m_pimpl->freeMemoryBlocks.push_back({ (unsigned char*)memoryBlock->ptr, m_pimpl->memoryBlockSize });
                memoryBlock->ptr = nullptr;
                memoryBlock->size = 0;
                ret = true;
            }
        }
        return ret;
    }

    size_t  SimpleFixedMemoryPool::getMemoryTotalSize() const {
        return m_pimpl->memoryTotalSize;
    }

    size_t SimpleFixedMemoryPool::getMemoryUsedSize() const {
        return m_pimpl->memoryUsedSize;
    }

    size_t SimpleFixedMemoryPool::getMemoryBlockSize() const {
        return m_pimpl->memoryBlockSize;
    }

    size_t SimpleFixedMemoryPool::getMemoryBlocksCount() const {
        return m_pimpl->memoryTotalSize / m_pimpl->memoryBlockSize;
    }

    size_t SimpleFixedMemoryPool::getFreeMemoryBlocksCount() const {
        return m_pimpl->freeMemoryBlocks.size();
    }

    size_t SimpleFixedMemoryPool::getUsedMemoryBlocksCount() const {
        return m_pimpl->usedMemoryBlocks.size();
    }
}

