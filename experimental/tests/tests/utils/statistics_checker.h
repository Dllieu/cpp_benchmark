#pragma once

#include <cstdint>
#include <queue>
#include <utils/macros.h>

namespace tests
{
    // TODO: Would be great to know if Objects of containers are being moved / copied too (e.g. resize vector, ExpectMovedElements(oldSize))
    class StatisticsChecker
    {
    public:
        StatisticsChecker();
        DELETE_COPY_MOVE_CONSTRUCTOR(StatisticsChecker);
        ~StatisticsChecker() = default;

        void IgnoreChecks();

        void ExpectAllocate(std::size_t iExpectedAllocatedBytes);
        void ExpectDeallocate(std::size_t iExpectedDeallocatedBytes);

        void RecordAllocate(std::size_t iAllocatedBytes);
        void RecordDeallocate(std::size_t iDeallocatedBytes);

    private:
        std::queue<std::size_t> m_ExpectedAllocatedBytes;
        std::queue<std::size_t> m_ExpectedDeallocatedBytes;
        bool m_IgnoreCheck;
    };
}
