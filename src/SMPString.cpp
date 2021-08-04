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
        if(m_memoryPool && str)
        {
            m_stringSize = strlen(str);
            m_buffer = m_memoryPool->allocateMemory(m_stringSize);
            std::strcpy(reinterpret_cast<char *>(m_buffer.ptr), str);
        }
    }

    SMPString::SMPString(SimpleFixedMemoryPool * memoryPool, size_t strSize) : m_buffer(), m_memoryPool(memoryPool)
    {
        if(m_memoryPool)
        {
            m_stringSize = strSize;
            m_buffer = m_memoryPool->allocateMemory(m_stringSize);
            m_buffer.ptr[0] = '\0';
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

    SMPString & SMPString::operator=(const char * str)
    {
        if(m_memoryPool && str)
        {
            if(strlen(str) > m_buffer.size)
            {
                m_memoryPool->freeMemory(&m_buffer);
                m_buffer = m_memoryPool->allocateMemory(strlen(str));
            }
            m_stringSize = strlen(str);
            std::strcpy(reinterpret_cast<char *>(m_buffer.ptr), str);
        }
        return * this;
    }

    char & SMPString::operator[](size_t index)
    {
        return reinterpret_cast<char *>(m_buffer.ptr)[index];
    }

    const char & SMPString::operator[](size_t index) const
    {
        return m_buffer.ptr[index];
    }

    bool SMPString::operator==(const SMPString & that)
    {
        return (0 == strcmp(this->getBuffer(), that.getBuffer()));
    }

    bool SMPString::operator==(const char * str)
    {
        return (0 == strcmp(this->getBuffer(), str));
    }

    SMPString SMPString::operator+(const SMPString & that) const
    {        
        SMPString ret(m_memoryPool, this->getStringSize() + that.getStringSize());
        std::strcpy(reinterpret_cast<char *>(ret.m_buffer.ptr), reinterpret_cast<char *>(m_buffer.ptr)); 
        std::strcpy(reinterpret_cast<char *>(ret.m_buffer.ptr + m_stringSize), reinterpret_cast<char *>(that.m_buffer.ptr));
        return ret;
    }

    SMPString SMPString::operator+(const char * str) const
    {
        SMPString ret(m_memoryPool, this->getStringSize() + strlen(str));
        std::strcpy(reinterpret_cast<char *>(ret.m_buffer.ptr), reinterpret_cast<char *>(m_buffer.ptr));
        std::strcpy(reinterpret_cast<char *>(ret.m_buffer.ptr + m_stringSize), str);
        return ret;
    }

    SMPString & SMPString::operator+=(const char * str)
    {
        size_t newStringSize = strlen(str) + getStringSize();
        if(newStringSize > getBufferSize())
        {
            auto newBuffer = m_memoryPool->allocateMemory(newStringSize);
            std::strcpy(reinterpret_cast<char *>(newBuffer.ptr), reinterpret_cast<char *>(m_buffer.ptr));
            m_memoryPool->freeMemory(&m_buffer);
            m_buffer = newBuffer;
        }
        std::strcpy(reinterpret_cast<char *>(m_buffer.ptr + m_stringSize), str);
        m_stringSize = newStringSize;
        return *this;
    }

    const char * SMPString::getBuffer() const
    {
        return reinterpret_cast<const char *>(m_buffer.ptr);
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