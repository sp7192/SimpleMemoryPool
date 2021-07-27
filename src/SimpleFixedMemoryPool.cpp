#include "SimpleFixedMemoryPool.h"

#include <algorithm>
#include <cstdio>

namespace SimpleMemoryPool
{
    struct SimpleFixedMemoryPool::MemoryBlockInfo
    {
        MemoryBlock memoryBlock;
        bool        isUsed;
        long long   id;

        MemoryBlockInfo() : memoryBlock(), isUsed(false), id(0)
        {}
    };

    SimpleFixedMemoryPool::SimpleFixedMemoryPool(size_t totalSize, size_t blockSize,
                                                 size_t distributedCount, MemoryDistributionPolicy distributionPolicy)
        : m_totalSize(totalSize), m_usedSize(0), m_blockSize(blockSize),
        m_blocksInfo(nullptr), m_startBlockPtr(nullptr), m_lastBlockId(0),
        m_distributedBlocksCount(distributedCount), m_distributionPolicy(distributionPolicy)
    {
        try
        {
            m_startBlockPtr = calloc(m_totalSize, sizeof(uint8_t));
        }
        catch(...)
        {
            printf("COULD NOT ALLOCATE %zu memory\n", m_totalSize);
            std::terminate();
        }
        if(m_blockSize > m_totalSize)
        {
            m_blockSize = m_totalSize;
        }
        if(0 == m_distributedBlocksCount)
        {
            m_distributedBlocksCount = 1;
        }
        m_blocksCount = m_freeBlocksCount = m_blockSize > 0 ? m_totalSize / m_blockSize : 0;
        m_blocksInfo = new MemoryBlockInfo[m_blocksCount];
        for(int i = 0; i < m_blocksCount; ++i)
        {
            m_blocksInfo[i].memoryBlock.ptr = reinterpret_cast<unsigned char *>(m_startBlockPtr) + i * m_blockSize;
            m_blocksInfo[i].memoryBlock.size = m_blockSize;
            m_blocksInfo[i].isUsed = false;
        }
    }

    SimpleFixedMemoryPool::~SimpleFixedMemoryPool()
    {
        if(m_startBlockPtr)
        {
            free(m_startBlockPtr);
            m_startBlockPtr = nullptr;
        }
        if(m_blocksInfo)
        {
            delete[] m_blocksInfo;
            m_blocksInfo = nullptr;
        }
    }

    size_t SimpleFixedMemoryPool::computeStartingAllocationIndex(size_t requestedBlocksCount) const
    {
        size_t distributedBlocksSize = m_blocksCount / m_distributedBlocksCount;
        return distributedBlocksSize * ((requestedBlocksCount - 1) / (distributedBlocksSize / m_distributedBlocksCount));
    }

    MemoryBlock SimpleFixedMemoryPool::allocateMemory()
    {
        MemoryBlock ret;
        if(m_freeBlocksCount > 0)
        {
            for(size_t i = 0; i < m_blocksCount; ++i)
            {
                if(!m_blocksInfo[i].isUsed)
                {
                    ret = m_blocksInfo[i].memoryBlock;
                    m_blocksInfo[i].isUsed = true;
                    m_blocksInfo[i].id = ++m_lastBlockId;
                    m_usedSize += m_blockSize;
                    m_freeBlocksCount--;
                    break;
                }
            }
        }
        return ret;
    }

    MemoryBlock SimpleFixedMemoryPool::allocateMemory(size_t size)
    {
        MemoryBlock ret;
        size_t requestedBlocksCount = (size + m_blockSize - 1) / m_blockSize;

        if(m_freeBlocksCount >= requestedBlocksCount &&
            (MemoryDistributionPolicy::None == m_distributionPolicy || m_blocksCount/ m_distributedBlocksCount >= requestedBlocksCount))
        {
            size_t i = 0;
            if(MemoryDistributionPolicy::None != m_distributionPolicy)
            {
                i = computeStartingAllocationIndex(requestedBlocksCount);
            }
            while(i < m_blocksCount && (requestedBlocksCount <= (m_blocksCount - i)))
            {
                bool hasFreeBlocks = false;
                auto beginPtr = m_blocksInfo + i;
                auto endPtr = m_blocksInfo + i + requestedBlocksCount;
                auto it = std::find_if(beginPtr, endPtr, [](MemoryBlockInfo & memInfo) {
                    return memInfo.isUsed;
                });
                hasFreeBlocks = (it == endPtr);
                if(hasFreeBlocks)
                {
                    ret = m_blocksInfo[i].memoryBlock;
                    ret.size = m_blockSize * requestedBlocksCount;
                    ++m_lastBlockId;
                    std::for_each(beginPtr, endPtr, [this](MemoryBlockInfo & memInfo) {
                        memInfo.isUsed = true;
                        memInfo.id = m_lastBlockId;
                    });
                    m_usedSize += ret.size;
                    m_freeBlocksCount -= requestedBlocksCount;
                    break;
                }
                ++i;
            }
        }
        return ret;
    }

    bool SimpleFixedMemoryPool::freeMemory(MemoryBlock * memoryBlock)
    {
        bool ret = false;
        auto endPtr = m_blocksInfo + m_blocksCount;
        if(memoryBlock && memoryBlock->ptr && m_freeBlocksCount != m_blocksCount)
        {
            auto firstItemIter = std::find_if(m_blocksInfo, endPtr, [memoryBlock] (MemoryBlockInfo & memInfo) {
                return memInfo.isUsed && memInfo.memoryBlock.ptr == memoryBlock->ptr;
            });
            if(firstItemIter != endPtr)
            {
                auto id = firstItemIter->id;
                std::for_each(firstItemIter, endPtr, [&](MemoryBlockInfo & memInfo) {
                    if (memInfo.id == id)
                    {
                        m_usedSize -= m_blockSize;
                        memInfo.isUsed = false;
                        memInfo.id = 0;
                        ++m_freeBlocksCount;
                    }
                    else
                    {
                        return;
                    }
                });
                
                memset(memoryBlock->ptr, 0, memoryBlock->size);
                memoryBlock->ptr = nullptr;
                memoryBlock->size = 0;

                ret = true;
            }
        }
        return ret;
    }

    size_t  SimpleFixedMemoryPool::getMemoryTotalSize() const
    {
        return m_totalSize;
    }

    size_t SimpleFixedMemoryPool::getMemoryUsedSize() const
    {
        return m_usedSize;
    }

    size_t SimpleFixedMemoryPool::getMemoryBlockSize() const
    {
        return m_blockSize;
    }

    size_t SimpleFixedMemoryPool::getMemoryBlocksCount() const
    {
        return m_blocksCount;
    }

    size_t SimpleFixedMemoryPool::getFreeMemoryBlocksCount() const
    {
        return m_freeBlocksCount;
    }

    size_t SimpleFixedMemoryPool::getUsedMemoryBlocksCount() const
    {
        return m_blocksCount - m_freeBlocksCount;
    }

    void SimpleFixedMemoryPool::logMemory() const
    {
        printf("================\n");
        printf("Total Memory size : %zu, usedSize Mem : %zu\n", getMemoryTotalSize(), getMemoryUsedSize());
        printf("Total Memory Blocks Count : %zu, Used Memory Blocks Count : %zu,"
                "Free Memory Blocks Count : %zu\n", getMemoryBlocksCount(),
               getMemoryBlocksCount() - getFreeMemoryBlocksCount(), getFreeMemoryBlocksCount());
        printf("================\n");
        
        for(int i = 0; i < getMemoryBlocksCount(); ++i)
        {
            char buffer[8192] = { 0 };
            int offset = 0;
            while(offset < sizeof(buffer) && i < getMemoryBlocksCount())
            {
                offset += snprintf(offset + buffer, sizeof(buffer) - offset,
                                   "Block[%d] = %s; id = %zu; ptr = %p\n", i, m_blocksInfo[i].isUsed ? "USED" : "FREE", m_blocksInfo[i].id,
                                   m_blocksInfo[i].isUsed ? m_blocksInfo[i].memoryBlock.ptr : nullptr);
                ++i;
            }
            --i;
            printf("%s\n", buffer);
        }
    }
}

