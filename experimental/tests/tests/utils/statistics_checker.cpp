#include <gtest/gtest.h>
#include <utils/statistics_checker.h>
#include <cstddef>

using namespace tests;

StatisticsChecker::StatisticsChecker()
    : m_ExpectedAllocatedBytes(),
      m_ExpectedDeallocatedBytes(),
      m_IgnoreCheck(false)
{
}

void StatisticsChecker::IgnoreChecks()
{
    EXPECT_EQ(true, this->m_ExpectedAllocatedBytes.empty());
    EXPECT_EQ(true, this->m_ExpectedDeallocatedBytes.empty());

    this->m_IgnoreCheck = true;
}

void StatisticsChecker::ExpectAllocate(std::size_t iExpectedAllocatedBytes)
{
    this->m_ExpectedAllocatedBytes.push(iExpectedAllocatedBytes);
}

void StatisticsChecker::ExpectDeallocate(std::size_t iExpectedDeallocatedBytes)
{
    this->m_ExpectedDeallocatedBytes.push(iExpectedDeallocatedBytes);
}

void StatisticsChecker::RecordAllocate(std::size_t iAllocatedBytes)
{
    if (true == this->m_IgnoreCheck)
    {
        return;
    }

    EXPECT_EQ(false, this->m_ExpectedAllocatedBytes.empty());
    EXPECT_EQ(iAllocatedBytes, this->m_ExpectedAllocatedBytes.front());

    this->m_ExpectedAllocatedBytes.pop();
}

void StatisticsChecker::RecordDeallocate(std::size_t iDeallocatedBytes)
{
    if (true == this->m_IgnoreCheck)
    {
        return;
    }

    EXPECT_EQ(false, this->m_ExpectedDeallocatedBytes.empty());
    EXPECT_EQ(iDeallocatedBytes, this->m_ExpectedDeallocatedBytes.front());

    this->m_ExpectedDeallocatedBytes.pop();
}
