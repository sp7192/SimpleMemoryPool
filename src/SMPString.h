#include "SimpleFixedMemoryPool.h"

namespace SimpleMemoryPool
{
    // TODO : Maybe changing class name.
    class SMPString
    {
        MemoryBlock             m_buffer;
        const size_t            m_defaultBufferSize = 32;
        size_t                  m_stringSize = 0;
        SimpleFixedMemoryPool * m_memoryPool = nullptr;

    public :
        SMPString(SimpleFixedMemoryPool * memoryPool);
        SMPString(SimpleFixedMemoryPool * memoryPool, const char * str);
        ~SMPString();

        SMPString(const SMPString & that);
        SMPString & operator=(const SMPString & that);
        SMPString(const SMPString && that) noexcept;
        SMPString & operator=(const SMPString && that) noexcept;

        SMPString & operator=(const char * str);
        char & operator[](size_t index);
        const char & operator[](size_t index) const;

        // TODO : Maybe changing method name.
        const char * getBuffer() const;
        size_t getBufferSize() const;
        size_t getStringSize() const;
    };
}