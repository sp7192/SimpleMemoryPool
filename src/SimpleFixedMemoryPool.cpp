#include "SimpleFixedMemoryPool.h"

#include <algorithm>
#include <cstdio>
#include <vector>

namespace SimpleMemoryPool
{
    struct SimpleFixedMemoryPool::MemoryBlockInfo
    {
        MemoryBlock memoryBlock;
        bool        isUsed;

        MemoryBlockInfo() : memoryBlock(), isUsed(false)
        {}
    };

    SimpleFixedMemoryPool::SimpleFixedMemoryPool(size_t totalSize, size_t blockSize)
        : m_totalSize(totalSize), m_usedSize(0), m_blockSize(blockSize),
        m_blocksInfo(nullptr), m_startBlockPtr(nullptr)
    {
        try
        {
            m_startBlockPtr = malloc(m_totalSize);
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
        m_blocksCount = m_freeBlocksCount = m_blockSize > 0 ? m_totalSize / m_blockSize : 0;
        m_blocksInfo = new MemoryBlockInfo[m_blocksCount];
        for(int i = 0; i < m_blocksCount; ++i)
        {
            m_blocksInfo[i].memoryBlock.ptr = (unsigned char *)m_startBlockPtr + i * m_blockSize;
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
                    m_usedSize += m_blockSize;
                    m_freeBlocksCount--;
                    break;
                }
            }
        }
        return ret;
    }

    bool SimpleFixedMemoryPool::freeMemory(MemoryBlock * memoryBlock)
    {
        bool ret = false;
        if(memoryBlock && memoryBlock->ptr && m_freeBlocksCount != m_blocksCount)
        {
            for(size_t i = 0; i < m_blocksCount; ++i)
            {
                if(m_blocksInfo[i].isUsed && m_blocksInfo[i].memoryBlock.ptr == memoryBlock->ptr)
                {
                    m_usedSize -= m_blockSize;
                    m_blocksInfo[i].isUsed = false;
                    memoryBlock->ptr = nullptr;
                    memoryBlock->size = 0;
                    m_freeBlocksCount++;
                    ret = true;
                }
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
}

