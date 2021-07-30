#include "SMPString.h"
#include <algorithm>

namespace SimpleMemoryPool
{
    SMPString::SMPString(SimpleFixedMemoryPool * memoryPool) : m_buffer(), m_memoryPool(memoryPool)
    {
        if(m_memoryPool)
        {
            m_buffer = m_memoryPool->allocateMemory(m_defaultBufferSize);
        }
    }

    SMPString::SMPString(SimpleFixedMemoryPool * memoryPool, const char * str) : m_buffer(), m_memoryPool(memoryPool)
    {
        if(m_memoryPool)
        {
            m_stringSize = strlen(str);
            m_buffer = m_memoryPool->allocateMemory(m_stringSize);
            std::strcpy(reinterpret_cast<char *>(m_buffer.ptr), str);
        }
    }

    SMPString::~SMPString()
    {
        if(m_memoryPool)
        {
            m_memoryPool->freeMemory(&m_buffer);
        }
    }

    SMPString::SMPString(const SMPString & that) : m_buffer(), m_memoryPool(that.m_memoryPool)
    {
        if(m_memoryPool)
        {
            m_stringSize = that.m_stringSize;
            m_buffer = m_memoryPool->allocateMemory(m_stringSize);
            std::memcpy(m_buffer.ptr, that.m_buffer.ptr, that.m_buffer.size);
        }
    }

    SMPString & SMPString::operator=(const SMPString & that)
    {
        if(that.m_memoryPool && m_buffer.ptr != that.m_buffer.ptr)
        {
            if(m_memoryPool)
            {
                m_memoryPool->freeMemory(&m_buffer);
            }

            m_memoryPool = that.m_memoryPool;
            m_stringSize = that.m_stringSize;
            m_buffer = m_memoryPool->allocateMemory(m_stringSize);
            std::memcpy(m_buffer.ptr, that.m_buffer.ptr, that.m_buffer.size);
        }
        return *this;
    }

    SMPString::SMPString(const SMPString && that) noexcept : m_buffer(that.m_buffer), m_stringSize(that.m_stringSize), m_memoryPool(that.m_memoryPool)
    {
    }

    SMPString & SMPString::operator=(const SMPString && that) noexcept
    {
        m_buffer = that.m_buffer;
        m_stringSize = that.m_stringSize;
        m_memoryPool = that.m_memoryPool;
        return *this;
    }

    char const * SMPString::getBuffer() const
    {
        return reinterpret_cast<char const *>(m_buffer.ptr);
    }

    size_t SMPString::getBufferSize() const
    {
        return m_buffer.size;
    }

    size_t SMPString::getStringSize() const
    {
        return m_stringSize;
    }

}