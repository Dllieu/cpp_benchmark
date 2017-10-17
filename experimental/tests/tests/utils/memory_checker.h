#pragma once

#include <queue>
#include <cstddef>

namespace tests
{
    class MemoryChecker
    {
    public:
        MemoryChecker();
        ~MemoryChecker() = default;

        void IgnoreChecks();

        void ExpectAllocate(std::size_t iExpectedAllocatedBytes);
        void ExpectDeallocate(std::size_t iExpectedDeallocatedBytes);

        void CheckAllocate(std::size_t iAllocatedBytes);
        void CheckDeallocate(std::size_t iDeallocatedBytes);

    private:
        std::queue<std::size_t> m_ExpectedAllocatedBytes;
        std::queue<std::size_t> m_ExpectedDeallocatedBytes;
        bool m_IgnoreCheck;
    };
}
