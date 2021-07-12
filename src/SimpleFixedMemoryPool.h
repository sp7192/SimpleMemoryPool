#pragma once

#include <utility>
#include <new>

#include "MemoryBlock.h"

namespace SimpleMemoryPool
{
    class SimpleFixedMemoryPool
    {
        size_t            m_totalSize;
        size_t            m_usedSize;
        size_t            m_blockSize;
        size_t            m_freeBlocksCount;
        size_t            m_blocksCount;
        void *            m_startBlockPtr;

        struct MemoryBlockInfo;
        MemoryBlockInfo * m_blocksInfo;

    public:
        SimpleFixedMemoryPool(size_t totalSize, size_t chunckSize);
        ~SimpleFixedMemoryPool();

        SimpleFixedMemoryPool(const SimpleFixedMemoryPool &) = delete;
        SimpleFixedMemoryPool & operator=(const SimpleFixedMemoryPool &) = delete;
        SimpleFixedMemoryPool(const SimpleFixedMemoryPool &&) = delete;
        SimpleFixedMemoryPool & operator=(const SimpleFixedMemoryPool &&) = delete;

        MemoryBlock allocateMemory();
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
    };

    template<typename T, class ... Args>
    T * SimpleFixedMemoryPool::construct(Args && ... args)
    {
        T * ret = nullptr;
        MemoryBlock mem = allocateMemory();
        if(mem.ptr && mem.size >= sizeof(T))
        {
            ret = new (mem.ptr) T(std::forward<Args>(args)...);
        }
        else if(mem.ptr)
        {
            freeMemory(&mem);
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
            MemoryBlock memoryBlock((unsigned char *)(*ptr), getMemoryBlockSize());
            ret = freeMemory(&memoryBlock);
            *ptr = (T *)memoryBlock.ptr;
        }
        return ret;
    }

    template<typename T, class ... Args>
    ArrayBlock<T> SimpleFixedMemoryPool::constructArray(size_t count, Args && ... args)
    {
// TODO : Does not check if all blocks of a contiguous memory are free and need to change.
        ArrayBlock<T> ret;
        if(sizeof(T) * count <= getMemoryBlockSize())
        {
            MemoryBlock mem = allocateMemory();
            if(mem.ptr)
            {
                ret.ptr = (T *)mem.ptr;
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
                    ret.ptr = (T *)mem.ptr;
                    ret.count = count;
                }
                else
                {
                    ret.ptr = nullptr;
                    ret.count = 0;
                }
            }
        }
        else
        {
            // TODO : to be implemented
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
            MemoryBlock memoryBlock((unsigned char *)(array->ptr), getMemoryBlockSize());
            ret = freeMemory(&memoryBlock);
            array->ptr = (T *)memoryBlock.ptr;
            array->count = 0;
        }
        return ret;
    }
}
