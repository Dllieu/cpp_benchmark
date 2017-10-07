#pragma once

namespace allocators
{
    typename <std::size_t BlockSize>
    class MemoryPoolResource
    {
        struct FreeList
        {
            FreeList* m_Next;
        };
    };
}
