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
            m_buffer = m_memoryPool->allocateMemory(strlen(str));
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
            m_buffer = m_memoryPool->allocateMemory(that.m_buffer.size);
            std::memcpy(m_buffer.ptr, that.m_buffer.ptr, that.m_buffer.size);
        }
    }

    SMPString & SMPString::operator=(const SMPString & that)
    {
        // TODO : Check for self assignment.
        if(that.m_memoryPool)
        {
            m_memoryPool = that.m_memoryPool;
            m_buffer = m_memoryPool->allocateMemory(that.m_buffer.size);
            std::memcpy(m_buffer.ptr, that.m_buffer.ptr, that.m_buffer.size);
        }
        return *this;
    }

    SMPString::SMPString(const SMPString && that) noexcept : m_buffer(that.m_buffer), m_memoryPool(that.m_memoryPool)
    {
    }

    SMPString & SMPString::operator=(const SMPString && that) noexcept
    {
        m_buffer = that.m_buffer;
        m_memoryPool = that.m_memoryPool;
        return *this;
    }

    char const * SMPString::getBuffer() const
    {
        return reinterpret_cast<char const *>(m_buffer.ptr);
    }

}