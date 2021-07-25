#pragma once

#include <utility>
#include <new>

#include "MemoryBlock.h"

namespace SimpleMemoryPool
{
    enum class MemoryDistributionPolicy
    {
        None,
        FourCloseRanges,
        FourOpenRanges
    };

    class SimpleFixedMemoryPool
    {
        size_t                      m_totalSize;
        size_t                      m_usedSize;
        size_t                      m_blockSize;
        size_t                      m_freeBlocksCount;
        size_t                      m_blocksCount;
        size_t                      m_lastBlockId;
        void *                      m_startBlockPtr;
        MemoryDistributionPolicy    m_distributionPolicy;

        struct MemoryBlockInfo;
        MemoryBlockInfo * m_blocksInfo;

    public:
        SimpleFixedMemoryPool(size_t totalSize, size_t chunckSize, MemoryDistributionPolicy distributionPolicy = MemoryDistributionPolicy::None);
        ~SimpleFixedMemoryPool();

        SimpleFixedMemoryPool(const SimpleFixedMemoryPool &) = delete;
        SimpleFixedMemoryPool & operator=(const SimpleFixedMemoryPool &) = delete;
        SimpleFixedMemoryPool(const SimpleFixedMemoryPool &&) = delete;
        SimpleFixedMemoryPool & operator=(const SimpleFixedMemoryPool &&) = delete;

        MemoryBlock allocateMemory();
        MemoryBlock allocateMemory(size_t size);
        bool freeMemory(MemoryBlock * memoryBlock);

        template<typename T, class ... Args>
        T * construct(Args && ... args);
        template<typename T>
        bool destruct(T ** ptr);

        template<typename T, class ... Args>
        ArrayBlock<T> constructArray(size_t count, Args && ... args);
        template<typename T>
        bool destructArray(ArrayBlock<T> * ptr);

        size_t getMemoryTotalSize() const;
        size_t getMemoryUsedSize() const;
        size_t getMemoryBlockSize() const;
        size_t getMemoryBlocksCount() const;
        size_t getFreeMemoryBlocksCount() const;
        size_t getUsedMemoryBlocksCount() const;

        void logMemory() const;
    };

    template<typename T, class ... Args>
    T * SimpleFixedMemoryPool::construct(Args && ... args)
    {
        T * ret = nullptr;
        MemoryBlock mem = allocateMemory(sizeof(T));
        if(mem.ptr)
        {
            ret = new (mem.ptr) T(std::forward<Args>(args)...);
        }
        return ret;
    }

    template<typename T>
    bool SimpleFixedMemoryPool::destruct(T ** ptr)
    {
        bool ret = false;
        if(*ptr)
        {
            (*ptr)->~T();
            MemoryBlock memoryBlock((unsigned char *)(*ptr), sizeof(T));
            ret = freeMemory(&memoryBlock);
            *ptr = reinterpret_cast<T *>(memoryBlock.ptr);
        }
        return ret;
    }

    template<typename T, class ... Args>
    ArrayBlock<T> SimpleFixedMemoryPool::constructArray(size_t count, Args && ... args)
    {
        ArrayBlock<T> ret;
        MemoryBlock mem = allocateMemory(sizeof(T) * count);
        if(mem.ptr)
        {
            ret.ptr = reinterpret_cast<T *>(mem.ptr);
            T * elemPtr = nullptr;
            for(int i = 0; i < count; ++i)
            {
                elemPtr = new (ret.ptr + i) T(std::forward<Args>(args)...);
                if(!elemPtr)
                {
                    freeMemory(&mem);
                    break;
                }
            }
            if(elemPtr)
            {
                ret.ptr = reinterpret_cast<T *>(mem.ptr);
                ret.count = count;
            }
            else
            {
                ret.ptr = nullptr;
                ret.count = 0;
            }
        }
        return ret;
    }

    template<typename T>
    bool SimpleFixedMemoryPool::destructArray(ArrayBlock<T> * array)
    {
        bool ret = false;
        if(array->ptr)
        {
            for(int i = 0; i < array->count; ++i)
            {
                (*array)[i].~T();
            }
            MemoryBlock memoryBlock((unsigned char *)(array->ptr), array->count * sizeof(T));
            ret = freeMemory(&memoryBlock);
            array->ptr = reinterpret_cast<T *>(memoryBlock.ptr);
            array->count = 0;
        }
        return ret;
    }
}
