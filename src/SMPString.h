#include "SimpleFixedMemoryPool.h"

namespace SimpleMemoryPool
{
    // TODO : Maybe changing class name.
    class SMPString
    {
        MemoryBlock             m_buffer;
        const size_t            m_defaultBufferSize = 32;
        SimpleFixedMemoryPool * m_memoryPool = nullptr;

    public :
        SMPString(SimpleFixedMemoryPool * memoryPool);
        SMPString(SimpleFixedMemoryPool * memoryPool, const char * str);
        ~SMPString();

        SMPString(const SMPString & that);
        SMPString & operator=(const SMPString & that);
        SMPString(const SMPString && that) noexcept;
        SMPString & operator=(const SMPString && that) noexcept;

        // TODO : Maybe changing method name.
        char const * getBuffer() const;
    };
}